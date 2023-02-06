/*
 * Projection.cpp
 *
 *  Created on: Dec 9, 2022
 *      Author: Nathan
 */
#include <cmath>
#include <forward_list>
#include <iterator>
#include <algorithm>
#include <iostream>

#include "Projection.h"

#include "priori/Math.h"
#include "priori/Graphical.h"

using namespace std;
using namespace priori;
using namespace asp;

Vertex Vertex::operator+(const Vertex &other){
	Vertex out(*this);
	out.shade += other.shade;
	out.position += other.position;
	out.texel += other.texel;
	return out;
}

Vertex Vertex::operator-(const Vertex &other){
	Vertex out(*this);
	out.shade -= other.shade;
	out.position -= other.position;
	out.texel -= other.texel;
	return out;
}

Vertex Vertex::operator+=(const Vertex &other){
	*this = *this+other;
	return *this;
}

Vertex Vertex::operator-=(const Vertex &other){
	*this = *this-other;
	return *this;
}

Vertex Vertex::operator*(const double &d){
	Vertex out(*this);
	out.shade *= d;
	out.position *= d;
	out.texel *= d;
	return out;
}

Vertex Vertex::operator/(const double &d){
	Vertex out(*this);
	out.shade /= d;
	out.position /= d;
	out.texel /= d;
	return out;
}

Vertex Vertex::operator*=(const double &d){
	*this = *this*d;
	return *this;
}

Vertex Vertex::operator/=(const double &d){
	*this = *this/d;
	return *this;
}

Vertex& Triangle::operator[](const int &n){
	return points[n];
}

Color Texture::getColor(double x, double y){
	return image.pixels[(int)round(x*(image.width-1))][(int)round(y*(image.height-1))];
}

Scene::Scene(int width, int height) :
		depthInverse(new double*[width]),
		camera{Point3D(0, 0, 0), 0, 0, 0},
		viewPort(width, height),
		settings(),
		lights(){
	for(int i = 0; i < width; i++)
		depthInverse[i] = new double[height];

	clear();

	setFOV(90);
};

Scene::~Scene(){
	for(int i = 0; i < viewPort.width; i++)
		delete[] depthInverse[i];
	delete[] depthInverse;
}

void Scene::project(Triangle &triangle){
	for(int i = 0; i < 3; i++){
		triangle[i].position = Point3D((int)(viewPort.width/2+((triangle[i].position.x*focalLength)/triangle[i].position.z)),
									   (int)(viewPort.height/2-((triangle[i].position.y*focalLength)/triangle[i].position.z)),
									   1/triangle[i].position.z);
		triangle[i].texel *= triangle[i].position.z;
	}
}

void Scene::setFOV(double fov){
	focalLength = (viewPort.width-1)/(2*tan(fov*(M_PI/360)));
}

void Scene::drawTriangle(Triangle triangle){
	if(settings->wireframe){
		cout << "wireframe" << endl;
		priori::drawTriangle(viewPort, triangle.color, Point(triangle[0].position.x, triangle[0].position.y),
											  	  	   Point(triangle[1].position.x, triangle[1].position.y),
											  	  	   Point(triangle[2].position.x, triangle[2].position.y));
		viewPort[(int)triangle[0].position.x][(int)triangle[0].position.y] = 0xFF0000;
		viewPort[(int)triangle[1].position.x][(int)triangle[1].position.y] = 0xFF0000;
		viewPort[(int)triangle[2].position.x][(int)triangle[2].position.y] = 0xFF0000;
		return;
	}

	Vertex v0 = triangle[0], v1 = triangle[1], v2 = triangle[2];
	if(v0.position.y > v1.position.y)
		swap(v0, v1);
	if(v0.position.y > v2.position.y)
		swap(v0, v2);
	if(v1.position.y > v2.position.y)
		swap(v1, v2);

	int dy01 = v1.position.y-v0.position.y;
	int dy02 = v2.position.y-v0.position.y;
	int dy12 = v2.position.y-v1.position.y;

	forward_list<Vertex> l01 = lerp<Vertex>(0, v0, dy01, v1);
	forward_list<Vertex> l02 = lerp<Vertex>(0, v0, dy02, v2);
	forward_list<Vertex> l12 = lerp<Vertex>(0, v1, dy12, v2);

	auto it01 = l01.begin();
	auto it02 = l02.begin();
	auto it12 = l12.begin();

	for(int i = 0; i <= dy02; i++){
		Vertex v1 = *it02++;
		Vertex v2 = i < dy01 ? *it01++ : *it12++;
		if(v1.position.x > v2.position.x)
			swap(v1, v2);

		forward_list<Vertex> line = lerp<Vertex>(v1.position.x, v1, v2.position.x, v2);
		int y = v0.position.y+i;
		for(Vertex v: line){
			int x = v.position.x;

			if(x < 0 || x >= viewPort.width || y < 0 || y >= viewPort.height)
				cout << "out " << x << " " << y << endl;

			if(v.position.z > depthInverse[x][y]){
				depthInverse[x][y] = v.position.z;

				Color base = settings->textures && triangle.texture != nullptr ?
						triangle.texture->getColor(v.texel.x/depthInverse[x][y], v.texel.y/depthInverse[x][y]) :
						triangle.color;

				Color light = settings->shading == settings->Phong ?
						shade(Point3D(x/(focalLength*v.position.z), y/(focalLength*v.position.z), 1/v.position.z), Vector3D(v.shade.r, v.shade.g, v.shade.b)) :
						v.shade;

				viewPort[x][y] = base*light;
			}
		}
	}
}

void Scene::render(const Model &model, priori::TransformationMatrix transform){
	cout << "render" << endl;

	Model triangles = transformModel(model, transform);
	cout << "transformed" << endl;

	cull(triangles);
	cout << "culled" << endl;

	for(Triangle &t: triangles)
		shadeVertices(t);
	cout << "shaded" << endl;

	for(Triangle &t: triangles)
		project(t);
	cout << "projected" << endl;

	for(Triangle &t: triangles)
		drawTriangle(t);
}

Model Scene::transformModel(const Model &model, TransformationMatrix transform){
	Model triangles;
	for(auto it = model.begin(); it != model.end(); it++){
		Triangle triangle(*it);
		for(int i = 0; i < 3; i++){
			TransformationMatrix transformation = transform*
												  rotateZ(camera.rz)*
												  rotateY(camera.ry)*
												  rotateX(camera.rx)*
												  translate(-camera.position.x, -camera.position.y, camera.position.z);
			triangle[i].position = transformation*triangle[i].position;
			triangle.normals[i] = transformation*triangle.normals[i];
		}
		triangles.push_front(triangle);
	}
	return triangles;
}

void Scene::cull(Model &triangles){

	double xAngle = atan((viewPort.width-1)/(2.0*focalLength));
	double yAngle = atan((viewPort.height-1)/(2.0*focalLength));

	double x = sin(xAngle), y = sin(yAngle);

	Plane cullingPlanes[] = {Plane(Vector3D(0, 0, 1), -1),
							 Plane(Vector3D(x, 0, cos(xAngle)), 0),
							 Plane(Vector3D(-x, 0, cos(xAngle)), 0),
							 Plane(Vector3D(0, y, cos(yAngle)), 0),
							 Plane(Vector3D(0, -y, cos(yAngle)), 0)};

	for(Plane plane: cullingPlanes){
		auto prev = triangles.before_begin();
		for(auto it = triangles.begin(); it != triangles.end(); prev = it++){
			Triangle t = *it;
			int culled[3];
			int numCulled = 0;
			for(int i = 0; i < 3; i++){
				Point3D pos = t[i].position;
				if(plane.distance(pos) < 0)
					culled[numCulled++] = i;
				else
					culled[2-i+numCulled] = i;
			}

			if(numCulled == 3){
				triangles.erase_after(prev);
				it = prev;
			}
			else if(numCulled == 2){
				it->points[culled[0]] = lerp<Vertex>(plane.intersectionPercent(t[culled[0]].position, t[culled[2]].position), t[culled[0]], t[culled[2]]);
				it->points[culled[1]] = lerp<Vertex>(plane.intersectionPercent(t[culled[1]].position, t[culled[2]].position), t[culled[1]], t[culled[2]]);
			}
			else if(numCulled == 1){
				Triangle clipping(t);
				clipping[0] = lerp<Vertex>(plane.intersectionPercent(t[culled[0]].position, t[culled[1]].position), t[culled[0]], t[culled[1]]);
				clipping[1] = lerp<Vertex>(plane.intersectionPercent(t[culled[0]].position, t[culled[2]].position), t[culled[0]], t[culled[2]]);
				clipping[2] = t[culled[1]];

				it->points[culled[0]] = clipping[1];

				triangles.insert_after(prev, clipping);
			}
		}
	}
}

void Scene::shadeVertices(Triangle &triangle){
	if(settings->shading == settings->Flat){
		triangle[0].shade = shade(triangle[0].position, triangle.normals[0]);
		triangle[1].shade = triangle[0].shade;
		triangle[2].shade = triangle[0].shade;
	}
	else if(settings->shading == settings->Gouraund){
		triangle[0].shade = shade(triangle[0].position, triangle.normals[0]);
		triangle[1].shade = shade(triangle[1].position, triangle.normals[1]);
		triangle[2].shade = shade(triangle[2].position, triangle.normals[2]);
	}
	else if(settings->shading == settings->Phong){
		triangle[0].shade = Color(triangle.normals[0].x*0xFF, triangle.normals[0].y*0xFF, triangle.normals[0].z*0xFF);
		triangle[1].shade = Color(triangle.normals[1].x*0xFF, triangle.normals[1].y*0xFF, triangle.normals[1].z*0xFF);
		triangle[2].shade = Color(triangle.normals[2].x*0xFF, triangle.normals[2].y*0xFF, triangle.normals[2].z*0xFF);
	}
}

Color Scene::shade(Point3D point, Vector3D normal){
	Color c = 0;
	for(LightSource* light: lights){


	}
	return c;
}

void Scene::clear(){
	for(int i = 0; i < viewPort.width; i++){
		for(int j = 0; j < viewPort.height; j++){
			viewPort.pixels[i][j] = 0xFFFFFF;
			depthInverse[i][j] = 0;
		}
	}
}

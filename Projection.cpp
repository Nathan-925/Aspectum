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
		camera{Point3D(0, 0, 0), Vector3D(0, 0, 0)},
		viewPort(width, height),
		settings(){
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
	double xAngle = fov*(M_PI/360);
	focalLength = (viewPort.width-1)/(2*tan(xAngle));
	double yAngle = atan((viewPort.height-1)/(2.0*focalLength));

	double x = sin(xAngle), y = sin(yAngle);

	cullingPlanes.clear();
	cullingPlanes.push_front(Plane(Vector3D(0, 0, 1), 0));
	cullingPlanes.push_front(Plane(Vector3D(x, 0, cos(xAngle)), 0));
	cullingPlanes.push_front(Plane(Vector3D(-x, 0, cos(xAngle)), 0));
	cullingPlanes.push_front(Plane(Vector3D(0, y, cos(yAngle)), 0));
	cullingPlanes.push_front(Plane(Vector3D(0, -y, cos(yAngle)), 0));
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
				viewPort[x][y] = base*v.shade;
			}
		}
	}
}

void Scene::render(const Instance &inst){
	cout << "render" << endl;

	Model triangles = transformInstance(inst);
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

Model Scene::transformInstance(const Instance &instance){
	Model triangles;
	for(auto it = instance.model->begin(); it != instance.model->end(); it++){
		Triangle triangle(*it);
		for(int i = 0; i < 3; i++)
			triangle[i].position = instance.transform*
								   translate(-camera.position.x, -camera.position.y, -camera.position.z)*
								   rotateZ(camera.rotation.z)*
								   rotateY(camera.rotation.y)*
								   rotateX(camera.rotation.x)*
								   translate(camera.position.x, camera.position.y, camera.position.z)*
								   triangle[i].position;
		triangles.push_front(triangle);
	}
	return triangles;
}

void Scene::cull(Model &triangles){
	for(auto planeIt = cullingPlanes.begin(); planeIt != cullingPlanes.end(); planeIt++){
		for(auto it = triangles.begin(); it != triangles.end(); it++){
			Triangle t = *it;
			int culled[3];
			int numCulled = 0;
			for(int i = 0; i < 3; i++){
				Point3D pos = t[i].position;
				if(planeIt->distance(pos) < 0)
					culled[numCulled++] = i;
				else
					culled[2-i+numCulled] = i;
			}

			if(numCulled == 3){
				*it = *triangles.begin();
				triangles.pop_front();
			}
			else if(numCulled == 2){
				it->points[culled[0]] = lerp<Vertex>(planeIt->intersectionPercent(*planeIt, t[culled[0]].position, t[culled[2]].position), t[culled[0]], t[culled[2]]);
				it->points[culled[1]] = lerp<Vertex>(planeIt->intersectionPercent(*planeIt, t[culled[1]].position, t[culled[2]].position), t[culled[1]], t[culled[2]]);
			}
			else if(numCulled == 1){
				Triangle clipping(t);
				clipping[0] = lerp<Vertex>(planeIt->intersectionPercent(*planeIt, t[culled[0]].position, t[culled[1]].position), t[culled[0]], t[culled[1]]);
				clipping[1] = lerp<Vertex>(planeIt->intersectionPercent(*planeIt, t[culled[0]].position, t[culled[2]].position), t[culled[0]], t[culled[2]]);
				clipping[2] = t[culled[1]];

				it->points[culled[0]] = clipping[1];

				triangles.insert_after(it, clipping);
				it++;
			}
		}
	}
}

void Scene::shadeVertices(Triangle &triangle){
	triangle[0].shade = 0xFFFFFF;
	triangle[1].shade = 0xFFFFFF;
	triangle[2].shade = 0xFFFFFF;
}

void Scene::clear(){
	for(int i = 0; i < viewPort.width; i++){
		for(int j = 0; j < viewPort.height; j++){
			viewPort.pixels[i][j] = 0xFFFFFF;
			depthInverse[i][j] = 0;
		}
	}
}

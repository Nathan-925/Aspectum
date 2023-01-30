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

Vertex Vertex::operator*(const Vertex &other){
	Vertex out(*this);
	out.shade *= other.shade;
	out.position *= other.position;
	out.texel *= other.texel;
	return out;
}

Vertex Vertex::operator/(const Vertex &other){
	Vertex out(*this);
	out.shade /= other.shade;
	out.position /= other.position;
	out.texel /= other.texel;
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

Vertex Vertex::operator*=(const Vertex &other){
	*this = *this*other;
	return *this;
}

Vertex Vertex::operator/=(const Vertex &other){
	*this = *this/other;
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
		camera(0, 0, focalLength),
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

Triangle Scene::project(Triangle &triangle){
	for(int i = 0; i < 3; i++){
		cout << "(" << triangle[i].position.x << ", " << triangle[i].position.y << ", " << triangle[i].position.z << ")->";
		triangle[i].position = Point3D(viewPort.width/2+((triangle[i].position.x*focalLength)/triangle[i].position.z),
				   					   viewPort.height/2-((triangle[i].position.y*focalLength)/triangle[i].position.z),
									   1/triangle[i].position.z);
		triangle[i].texel *= triangle[i].position.z;
		cout << "(" << triangle[i].position.x << ", " << triangle[i].position.y << ", " << triangle[i].position.z << ")" << endl;
	}
	return triangle;
}

void Scene::setFOV(double fov){
	focalLength = viewPort.width/(2*tan((fov*M_PI/360)));

	cullingPlanes.clear();
	cullingPlanes.push_front(Plane(Vector3D(0, 0, 1).normalize(), 0));
	cullingPlanes.push_front(Plane(Vector3D(viewPort.width/2, 0, focalLength).normalize(), 0));
	cullingPlanes.push_front(Plane(Vector3D(-viewPort.width/2, 0, focalLength).normalize(), 0));
	cullingPlanes.push_front(Plane(Vector3D(0, viewPort.height/2, focalLength).normalize(), 0));
	cullingPlanes.push_front(Plane(Vector3D(0, -viewPort.height/2, focalLength).normalize(), 0));
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

	cout << "texture " << hex << (triangle.texture->getColor(1, 1)) << dec << endl;

	Vertex v0 = triangle[0], v1 = triangle[1], v2 = triangle[2];
	if(v0.position.y > v1.position.y)
		swap(v0, v1);
	if(v0.position.y > v2.position.y)
		swap(v0, v2);
	if(v1.position.y > v2.position.y)
		swap(v1, v2);

	int dy01 = floor(v1.position.y-v0.position.y);
	int dy02 = floor(v2.position.y-v0.position.y);
	int dy12 = floor(v2.position.y-v1.position.y);

	forward_list<Vertex> l01 = lerp<Vertex>(0, v0, dy01, v1);
	forward_list<Vertex> l02 = lerp<Vertex>(0, v0, dy02, v2);
	forward_list<Vertex> l12 = lerp<Vertex>(0, v1, dy12, v2);
	l12.pop_front();

	auto it01 = l01.begin();
	auto it02 = l02.begin();
	auto it12 = l12.begin();

	for(int i = 0; i <= dy02; i++){
		Vertex v1 = *it02++;
		Vertex v2 = it01 != l01.end() ? *it01++ : *it12++;
		if(v1.position.x > v2.position.x)
			swap(v1, v2);

		v1.position.x = floor(v1.position.x);
		v2.position.x = floor(v2.position.x);
		forward_list<Vertex> line = lerp<Vertex>(v1.position.x, v1, v2.position.x, v2);
		int y = v0.position.y+i;
		for(Vertex v: line){
			int x = v.position.x;
			cout << "xy " << x << " " << y << endl;
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

	cout << distance(triangles.begin(), triangles.end()) << endl;

	shadeVertices(triangles);
	cout << "shaded" << endl;

	for(auto it = triangles.begin(); it != triangles.end(); it++){
		project(*it);
		drawTriangle(*it);
	}
}

Model Scene::transformInstance(const Instance &instance){
	Model triangles;
	for(auto it = instance.model->begin(); it != instance.model->end(); it++){
		Triangle triangle(*it);
		for(int i = 0; i < 3; i++)
			triangle[i].position = instance.transform*camera*triangle[i].position;
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
				if(distanceToPlane(pos, *planeIt) < 0)
					culled[numCulled++] = i;
				else
					culled[2-i+numCulled] = i;
			}
			cout << numCulled << endl;

			if(numCulled == 3){
				*it = *triangles.begin();
				triangles.pop_front();
			}
			else if(numCulled == 2){
				it->points[culled[0]] = lerp<Vertex>(intersectionPercent(*planeIt, t[culled[0]].position, t[culled[2]].position), t[culled[0]], t[culled[2]]);
				it->points[culled[1]] = lerp<Vertex>(intersectionPercent(*planeIt, t[culled[1]].position, t[culled[2]].position), t[culled[1]], t[culled[2]]);

				cout << (*it)[0].position.y << endl;
				cout << (*it)[1].position.y << endl;
			}
			else if(numCulled == 1){
				Triangle clipping(t);
				clipping[0] = lerp<Vertex>(intersectionPercent(*planeIt, t[culled[0]].position, t[culled[1]].position), t[culled[0]], t[culled[1]]);
				clipping[1] = lerp<Vertex>(intersectionPercent(*planeIt, t[culled[0]].position, t[culled[2]].position), t[culled[0]], t[culled[2]]);
				clipping[2] = t[culled[1]];

				it->points[culled[0]] = clipping[1];

				triangles.insert_after(it, clipping);
				it++;
				cout << "successful" << endl;
			}
		}
	}
}

void Scene::shadeVertices(Model &triangles){
	for(auto it = triangles.begin(); it != triangles.end(); it++){
		(*it)[0].shade = 0xFFFFFF;
		(*it)[1].shade = 0xFFFFFF;
		(*it)[2].shade = 0xFFFFFF;
	}
}

void Scene::clear(){
	for(int i = 0; i < viewPort.width; i++){
		for(int j = 0; j < viewPort.height; j++){
			viewPort.pixels[i][j] = 0xFFFFFF;
			depthInverse[i][j] = 0;
		}
	}
}

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
	return Vertex(*this+other);
}

Vertex Vertex::operator-=(const Vertex &other){
	return Vertex(*this-other);
}

Vertex Vertex::operator*=(const Vertex &other){
	return Vertex(*this*other);
}

Vertex Vertex::operator/=(const Vertex &other){
	return Vertex(*this/other);
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
	return Vertex(*this*d);
}

Vertex Vertex::operator/=(const double &d){
	return Vertex(*this/d);
}

Vertex& Triangle::operator[](const int &n){
	return points[n];
}

Color Texture::getColor(double x, double y){
	return image.pixels[(int)(x*image.width)][(int)(y*image.height)];
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
		triangle[i].position = Point3D(viewPort.width/2+(triangle[i].position.x*focalLength)/triangle[i].position.z,
				   					   viewPort.height/2-(triangle[i].position.y*focalLength)/triangle[i].position.z,
									   1/triangle[i].position.z);
	}
	return triangle;
}

void Scene::setFOV(double fov){
	focalLength = viewPort.width/(2*tan((fov*M_PI/360)));

	cullingPlanes.clear();
	cullingPlanes.push_front(Plane(0, 0, 1, focalLength));
	cullingPlanes.push_front(Plane(viewPort.width/2, 0, focalLength, 0));
	cullingPlanes.push_front(Plane(-viewPort.width/2, 0, focalLength, 0));
	cullingPlanes.push_front(Plane(0, -viewPort.height/2, focalLength, 0));
	cullingPlanes.push_front(Plane(0, viewPort.height/2, focalLength, 0));
}

Vertex Scene::clipLine(Vertex culled, Vertex notCulled){
	for(auto it = cullingPlanes.begin(); it != cullingPlanes.end(); it++){
		if(distanceToPlane(culled.position, *it) < 0){
			double d = intersectionPercent(*it, culled.position, notCulled.position);
			culled = lerp<Vertex>(d, culled, notCulled);
		}
	}
	return culled;
}

void Scene::drawTriangle(Triangle triangle){
	if(settings->wireframe){
		cout << "wireframe" << endl;
		priori::drawTriangle(viewPort, triangle.color, Point(triangle[0].position.x, triangle[0].position.y),
											  	  	   Point(triangle[1].position.x, triangle[1].position.y),
											  	  	   Point(triangle[2].position.x, triangle[2].position.y));
		return;
	}

	cout << "draw" << endl;

	Vertex v0 = triangle[0], v1 = triangle[1], v2 = triangle[2];
	if(v0.position.y > v1.position.y)
		swap(v0, v1);
	if(v0.position.y > v2.position.y)
		swap(v0, v2);
	if(v1.position.y > v2.position.y)
		swap(v1, v2);

	int dy01 = round(v1.position.y-v0.position.y);
	int dy02 = round(v2.position.y-v0.position.y);
	int dy12 = round(v2.position.y-v1.position.y);

	forward_list<Vertex> l01 = lerp<Vertex>(0, v0, dy01, v1);
	forward_list<Vertex> l02 = lerp<Vertex>(0, v0, dy02, v2);
	forward_list<Vertex> l12 = lerp<Vertex>(0, v1, dy12, v2);
	l12.pop_front();

	auto it01 = l01.begin();
	auto it02 = l02.begin();
	auto it12 = l12.begin();

	cout << dy01 << endl;
	cout << dy02 << endl;
	cout << dy12 << endl;
	cout << endl;
	cout << v0.position.y << endl;
	cout << v1.position.y << endl;
	cout << v2.position.y << endl;
	for(int i = 0; i < dy02; i++){
		Vertex v1 = *it02++;
		Vertex v2 = it01 != l01.end() ? *it01++ : *it12++;
		if(v1.position.x > v2.position.x)
			swap(v1, v2);

		v1.position.x = round(v1.position.x);
		v2.position.x = round(v2.position.x);
		cout << "x pos " << v1.position.x << " " << v2.position.x << endl;
		forward_list<Vertex> line = lerp<Vertex>(v1.position.x, v1, v2.position.x, v2);
		int y = v0.position.y+i;
		for(auto it = line.begin(); it != line.end(); it++){
			int x = (*it).position.x;
			if((*it).position.z > depthInverse[x][y]){
				depthInverse[x][y] = (*it).position.z;

				Color base = settings->textures ? triangle.texture->getColor((*it).texel.x, (*it).texel.y) : triangle.color;
				viewPort[x][y] = base*(*it).shade;
				cout << x << " " << y << " " << hex << viewPort[x][y] << dec << endl;
			}
		}
	}
}

void Scene::render(const Instance &inst){
	cout << "render" << endl;

	Model triangles = transformModel(*inst.model);
	cout << "transformed" << endl;

	cull(triangles);
	cout << "culled" << endl;

	shadeVertices(triangles);
	cout << "shaded" << endl;

	for(auto it = triangles.begin(); it != triangles.end(); it++){
		cout << "not projected " << (*it)[0].position.y << endl;
		cout << "not projected " << (*it)[1].position.y << endl;
		cout << "not projected " << (*it)[2].position.y << endl;
		project(*it);
		cout << "projected " << (*it)[0].position.y << endl;
		cout << "projected " << (*it)[1].position.y << endl;
		cout << "projected " << (*it)[2].position.y << endl;
		drawTriangle(*it);
	}
}

Model Scene::transformModel(Model &model){
	Model triangles;
	for(auto it = model.begin(); it != model.end(); it++){
		Triangle triangle = *it;
		cout << (*it)[0].position.y << endl;
		cout << (*it)[1].position.y << endl;
		cout << (*it)[2].position.y << endl;
		cout << "pre transform " << triangle[0].position.y << endl;
		cout << "pre transform " << triangle[1].position.y << endl;
		cout << "pre transform " << triangle[2].position.y << endl;
		for(int i = 0; i < 3; i++)
			triangle[i].position = camera.transform(triangle[i].position);
		triangles.push_front(triangle);

		cout << "transform " << triangle[0].position.y << endl;
		cout << "transform " << triangle[1].position.y << endl;
		cout << "transform " << triangle[2].position.y << endl;
	}
	return triangles;
}

void Scene::cull(Model &triangles){
	for(auto it = triangles.begin(); it != triangles.end(); it++){
		int culled[3];
		int numCulled = 0;
		for(int i = 0; i < 3; i++){
			bool cull = false;
			for(auto planeIt = cullingPlanes.begin(); planeIt != cullingPlanes.end(); planeIt++){
				if(distanceToPlane((*it)[i].position, *planeIt) < 0){
					cull = true;
					break;
				}
			}
			if(cull)
				culled[numCulled++] = i;
			else
				culled[3-i-numCulled] = i;
		}
		cout << numCulled << endl;

		if(numCulled == 3){
			*it = *triangles.begin();
			triangles.pop_front();
		}
		else if(numCulled == 2){
			(*it)[0] = clipLine((*it)[culled[0]], (*it)[culled[2]]);
			(*it)[1] = clipLine((*it)[culled[1]], (*it)[culled[2]]);
		}
		else if(numCulled == 1){
			Triangle clipping;
			clipping[1] = (*it)[culled[1]];
			clipping[2] = (*it)[culled[2]];
			clipping[0] = clipLine((*it)[culled[0]], (*it)[culled[1]]);

			(*it)[culled[0]] = clipLine((*it)[culled[0]], (*it)[culled[2]]);

			triangles.insert_after(it, clipping);
			it++;
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

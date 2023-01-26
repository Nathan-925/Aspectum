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
	out.position += other.position;
	out.texel += other.texel;
	out.normal += other.normal;
	return out;
}

Vertex Vertex::operator-(const Vertex &other){
	Vertex out(*this);
	out.position -= other.position;
	out.texel -= other.texel;
	out.normal -= other.normal;
	return out;
}

Vertex Vertex::operator*(const Vertex &other){
	Vertex out(*this);
	out.position *= other.position;
	out.texel *= other.texel;
	out.normal *= other.normal;
	return out;
}

Vertex Vertex::operator/(const Vertex &other){
	Vertex out(*this);
	out.position /= other.position;
	out.texel /= other.texel;
	out.normal /= other.normal;
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
	out.position *= d;
	out.texel *= d;
	out.normal *= d;
	return out;
}

Vertex Vertex::operator/(const double &d){
	Vertex out(*this);
	out.position /= d;
	out.texel /= d;
	out.normal /= d;
	return out;
}

Vertex Vertex::operator*=(const double &d){
	return Vertex(*this*d);
}

Vertex Vertex::operator/=(const double &d){
	return Vertex(*this/d);
}

Vertex Triangle::operator[](const int &n){
	return points[n];
}

Color Texture::getColor(double x, double y){
	return image.pixels[(int)(x*image.width)][(int)(y*image.height)];
}

Scene::Scene(int width, int height) :
		depthInverse(new double[width*height]),
		camera(0, 0, focalLength),
		settings(),
		viewPort(width, height),
		focalLength(width/2){
	cullingPlanes.push_front(priori::Plane(0, 0, 1, -focalLength));
	cullingPlanes.push_front(priori::Plane(width/2, 0, focalLength, 0));
	cullingPlanes.push_front(priori::Plane(-width/2, 0, focalLength, 0));
	cullingPlanes.push_front(priori::Plane(0, -height/2, focalLength, 0));
	cullingPlanes.push_front(priori::Plane(0, height/2, focalLength, 0));
};

Scene::~Scene(){
	delete[] depthInverse;
}

Point3D Scene::project(const Point3D &point){
	return  Point3D(viewPort.width/2+(point.x*focalLength)/point.z,
				   	viewPort.height/2-(point.y*focalLength)/point.z,
					1/point.z);
}

double Scene::getFocalLength(double fovDegrees){
	return viewPort.width/(2*tan((fovDegrees*M_PI/360)));
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
		priori::drawTriangle(viewPort, triangle[0].color, Point(triangle[0].position.x, triangle[0].position.y),
											  	  	  	  Point(triangle[1].position.x, triangle[1].position.y),
											  	  	  	  Point(triangle[2].position.x, triangle[2].position.y));
		return;
	}

	if(triangle[0].position.y > triangle[1].position.y)
		swap(triangle[0], triangle[1]);
	if(triangle[0].position.y > triangle[2].position.y)
		swap(triangle[0], triangle[2]);
	if(triangle[1].position.y > triangle[2].position.y)
		swap(triangle[1], triangle[2]);

	int dy01 = abs((int)triangle[1].position.y-(int)triangle[0].position.y);
	int dy02 = abs((int)triangle[2].position.y-(int)triangle[0].position.y);
	int dy12 = abs((int)triangle[2].position.y-(int)triangle[1].position.y);

	Vertex* l01 = lerp<Vertex>(0, triangle[0], dy01, triangle[1]);
	Vertex* l02 = lerp<Vertex>(0, triangle[0], dy02, triangle[2]);
	Vertex* l12 = lerp<Vertex>(0, triangle[1], dy12, triangle[2]);

	for(int i = 0; i <= dy02; i++){
		Vertex v1 = l02[i];
		Vertex v2 = i < dy01 ? l01[i] : l12[i-dy01];
		if(v1.position.x > v2.position.x)
			swap(v1, v2);


	}

	for(int i = 0; i < dy02; i++){
		int x1 = x02[i];
		int x2 = i < dy01 ? x01[i] : x12[i-dy01];
		long double z1 = z02[i];
		long double z2 = i < dy01 ? z01[i] : z12[i-dy01];
		if(x1 > x2){
			long double temp = x1;
			x1 = x2;
			x2 = temp;
			temp = z1;
			z1 = z2;
			z2 = temp;
		}
		long double* depth = lerp<long double>(x1, z1, x2, z2);
		for(int j = 0; j < x2-x1; j++){
			int ind = x1+j+viewPort.width*((int)points[0].y+i);
			if(depth[j] > depthInverse[ind]){
				depthInverse[ind] = depth[j];
				viewPort.pixels[x1+j][(int)points[0].y+i] = color;
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

	for(auto it = triangles.begin(); it != triangles.end(); it++){
		(*it)[0].position = project((*it)[0].position);
		(*it)[1].position = project((*it)[1].position);
		(*it)[2].position = project((*it)[2].position);
		drawTriangle(*it);
	}
}

void Scene::setRenderSettings(RenderSettings* settings){
	this->settings = settings;
}

Model Scene::transformModel(Model &model){
	Model triangles;
	for(auto it = model.begin(); it != model.end(); it++){
		Triangle triangle = *it;
		for(int i = 0; i < 3; i++)
			triangle[i].position = camera.transform(triangle[i].position);
		triangles.push_front(triangle);
	}
	return triangles;
}

void Scene::cull(Model &triangles){
	for(auto it = triangles.begin(); it != triangles.end(); it++){
		int culled[3];
		int numCulled;
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

void Scene::clear(){
	for(int i = 0; i < viewPort.width; i++){
		for(int j = 0; j < viewPort.height; j++){
			viewPort.pixels[i][j] = 0xFFFFFF;
			depthInverse[i] = 0;
		}
	}
}

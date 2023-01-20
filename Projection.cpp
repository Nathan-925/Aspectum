/*
 * Projection.cpp
 *
 *  Created on: Dec 9, 2022
 *      Author: Nathan
 */
#include <cmath>
#include <forward_list>
#include <iterator>

#include "Projection.h"
#include "Rasterizer.h"

#include "priori/Math.h"

using namespace std;
using namespace priori;
using namespace asp;

Color Texture::getColor(double x, double y){
	return image.pixels[(int)(x*image.width)][(int)(y*image.height)];
}

Scene::Scene(int width, int height) :
		depthInverse(new double[width*height]),
		focalLength(width/2),
		camera(0, 0, focalLength),
		settings(),
		viewPort(width, height){
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
	return Point3D(viewPort.width/2+(point.x*focalLength)/point.z,
				   viewPort.height/2-(point.y*focalLength)/point.z,
				   point.z);
}

double Scene::getFocalLength(double fovDegrees){
	return viewPort.width/(2*tan((fovDegrees*M_PI/360)));
}

void Scene::drawTriangle(Color &color, const Point3D &p1, const Point3D &p2, const Point3D &p3){
	if(settings->wireframe){
		Point3D p2d1 = project(p1);
		Point3D p2d2 = project(p2);
		Point3D p2d3 = project(p3);
		raster::drawTriangle(viewPort, color, Point(p2d1.x, p2d1.y),
											  Point(p2d2.x, p2d2.y),
											  Point(p2d3.x, p2d3.y));
		return;
	}

	Point3D points[] = {project(p1), project(p2), project(p3)};
	if(points[0].y > points[1].y){
		Point3D temp = points[0];
		points[0] = points[1];
		points[1] = temp;
	}
	if(points[0].y > points[2].y){
		Point3D temp = points[0];
		points[0] = points[2];
		points[2] = temp;
	}
	if(points[1].y > points[2].y){
		Point3D temp = points[1];
		points[1] = points[2];
		points[2] = temp;
	}

	int dy01 = abs((int)points[1].y-(int)points[0].y);
	int dy02 = abs((int)points[2].y-(int)points[0].y);

	long double* x01 = lerp<long double>(points[0].y, points[0].x, points[1].y, points[1].x);
	long double* x02 = lerp<long double>(points[0].y, points[0].x, points[2].y, points[2].x);
	long double* x12 = lerp<long double>(points[1].y, points[1].x, points[2].y, points[2].x);

	long double* z01 = lerp<long double>(points[0].y, 1/points[0].z, points[1].y, 1/points[1].z);
	long double* z02 = lerp<long double>(points[0].y, 1/points[0].z, points[2].y, 1/points[2].z);
	long double* z12 = lerp<long double>(points[1].y, 1/points[1].z, points[2].y, 1/points[2].z);

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
	Model triangles = transformModel(inst.model);


}

void Scene::setRenderSettings(RenderSettings* settings){
	this->settings = settings;
}

Model Scene::transformModel(Model* model){
	Model triangles;
	for(auto it = model->begin(); it != model->end(); it++){
		Vertex triangle[3];
		triangle[0] = (*it)[0];
		triangle[0].position = (*it)[0].position.transform(camera);
		triangle[1] = (*it)[1];
		triangle[1].position = (*it)[1].position.transform(camera);
		triangle[2] = (*it)[2];
		triangle[2].position = (*it)[2].position.transform(camera);
		triangles.push_front(triangle);
	}
	return triangles;
}

void cull(Model &triangles){
	for(auto it = triangles.begin(); it != triangles.end(); it++){
		Vertex culled[3];
		int numCulled;
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

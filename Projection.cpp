/*
 * Projection.cpp
 *
 *  Created on: Dec 9, 2022
 *      Author: Nathan
 */
#include <cmath>
#include <forward_list>

#include "Projection.h"

#include "priori/Math.h"

#include "Rasterizer.h"

using namespace priori;

namespace proj{

	Point3D* Instance::getVertices() const{
		Point3D* out = new Point3D[model->numVertices];
		for(int i = 0; i < model->numVertices; i++){
			out[i] = model->vertices[i].transform(transform);
		}
		return out;
	}

	int** Instance::getTriangles() const{
		return model->triangles;
	}

	int Instance::getNumVertices() const{
		return model->numVertices;
	}

	int Instance::getNumTriangles() const{
		return model->numTriangles;
	}

	Color Texture::getColor(double x, double y){
		return image[(int)(x*width) + width*((int)(y*height))];
	}
}

Scene::Scene(int width, int height) :
		viewPort(width, height),
		depthInverse(new double[width*height]),
		focalLength(width/2),
		camera(0, 0, focalLength),
		numCullingPlanes(5),
		cullingPlanes{priori::Plane(0, 0, 1, -focalLength),
					  priori::Plane(width/2, 0, focalLength, 0),
					  priori::Plane(-width/2, 0, focalLength, 0),
					  priori::Plane(0, -height/2, focalLength, 0),
					  priori::Plane(0, height/2, focalLength, 0)},
		settings() {};

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

Point3D Scene::clipLine(Point3D culled, Point3D notCulled){
	Point3D clipped = culled;
	for(int i = 0; i < numCullingPlanes; i++){
		if(distanceToUnitPlane(clipped, cullingPlanes[i]) < 0){
			clipped = intersectionToPlane(cullingPlanes[i], clipped, notCulled);
		}
	}
	return clipped;
}

void Scene::drawTriangle(Color &color, const Point3D &p1, const Point3D &p2, const Point3D &p3){
	if(settings->wireframe){
		Point3D p2d1 = project(p1);
		Point3D p2d2 = project(p2);
		Point3D p2d3 = project(p3);
		raster::drawTriangle(viewPort, portWidth, portHeight, color, p2d1.x, p2d1.y,
																	 p2d2.x, p2d2.y,
																	 p2d3.x, p2d3.y);
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
			int ind = x1+j+portWidth*((int)points[0].y+i);
			if(depth[j] > depthInverse[ind]){
				depthInverse[ind] = depth[j];
				viewPort[ind] = color.c;
			}
		}
	}
}

void Scene::render(const proj::Instance &inst){
	Point3D points[inst.getNumVertices()];
	for(int i = 0; i < inst.getNumVertices(); i++)
		points[i] = inst.getVertices()[i].transform(camera);

	std::forward_list<Triangle> triangles;
	for(int* t: inst.getTriangles()){

	}

	for(int i = 0; i < inst.getNumTriangles(); i++){
		int* triangle = inst.getTriangles()[i];
		int numCulled = 0;
		int numNotCulled = 0;
		int culled[3];
		int notCulled[3];

		for(int i = 0; i < 3; i++){
			int j;
			for(j = 0; j < numCullingPlanes; j++){
				if(distanceToUnitPlane(points[triangle[i]], cullingPlanes[j]) < 0){
					culled[numCulled++] = triangle[i];
					break;
				}
			}
			if(j == numCullingPlanes)
				notCulled[numNotCulled++] = triangle[i];
		}

		if(numCulled == 0){
			Color color(rand());
			drawTriangle(color, points[triangle[0]], points[triangle[1]], points[triangle[2]]);

		}
		else if(numCulled == 1){
			Color color(rand());
			Point3D intersect1 = clipLine(points[culled[0]], points[notCulled[0]]);
			Point3D intersect2 = clipLine(points[culled[0]], points[notCulled[1]]);
			drawTriangle(color, points[notCulled[0]], points[notCulled[1]], intersect1);
			drawTriangle(color, points[notCulled[1]], intersect1, intersect2);
		}
		else if(numCulled == 2){
			Color color(rand());
			Point3D intersect1 = clipLine(points[culled[0]], points[notCulled[0]]);
			Point3D intersect2 = clipLine(points[culled[1]], points[notCulled[0]]);
			drawTriangle(color, points[notCulled[0]], intersect1, intersect2);
		}
	}
}

void Scene::wireframe(const proj::Instance &inst){

}

void Scene::clear(){
	for(int i = 0; i < portWidth*portHeight; i++){
		viewPort[i] = 0xFFFFFF;
		depthInverse[i] = 0;
	}
}

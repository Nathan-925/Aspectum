/*
 * Projection.cpp
 *
 *  Created on: Dec 9, 2022
 *      Author: Nathan
 */
#include <cmath>

#include "Math3D.h"
#include "Projection.h"
#include "Rasterizer.h"

namespace proj{

	math3d::Point3D* Instance::getVertices() const{
		math3d::Point3D* out = new math3d::Point3D[model->numVertices];
		for(int i = 0; i < model->numVertices; i++){
			out[i] = model->vertices[i].transform(transform);
		}
		return out;
	}

	Triangle* Instance::getTriangles() const{
		return model->triangles;
	}

	int Instance::getNumVertices() const{
		return model->numVertices;
	}

	int Instance::getNumTriangles() const{
		return model->numTriangles;
	}
}

bool Scene::cull(math3d::Point3D point, double a, double b, double c, double d){
	return math3d::distanceToPlane(point, 0, 0, 1, focalLength) > 0 &&
		   math3d::distanceToPlane(point, atan2(focalLength, portWidth/2), 0, atan2(focalLength, portWidth/2), 0) > 0 &&
		   math3d::distanceToPlane(point, -atan2(focalLength, portWidth/2), 0, atan2(focalLength, portWidth/2), 0) > 0 &&
		   math3d::distanceToPlane(point, atan2(focalLength, portHeight/2), 0, atan2(focalLength, portHeight/2), 0) > 0 &&
		   math3d::distanceToPlane(point, -atan2(focalLength, portHeight/2), 0, atan2(focalLength, portHeight/2), 0) > 0;
}

math3d::Point2D Scene::project(math3d::Point3D &point){
	math3d::Point2D out;
	out.x = portWidth/2+(point.x*focalLength)/point.z;
	out.y = portHeight/2+(point.y*focalLength)/point.z;
	return out;
}

double Scene::getFocalLength(double fovDegrees){
	return portWidth/(2*tan((fovDegrees*M_PI/360)));
}

void Scene::render(const proj::Instance &inst){
	math3d::Point3D* points = new math3d::Point3D[inst.getNumVertices()];
	math3d::Point2D* points2d = new math3d::Point2D[inst.getNumVertices()];
	for(int i = 0; i < inst.getNumVertices(); i++){
		points[i] = inst.getVertices()[i].transform(camera);
		points2d[i] = project(points[i]);
	}
	long double culledPoints[4];
	for(int i = 0; i < inst.getNumTriangles(); i++){
		int renderPoints = 0;
		for(int j = 0; j < 3; j++){
			if(!(cull(points[inst.getTriangles()[i].arr[j]], 0, 0, 1, focalLength))){

			}
		}

		raster::fillTriangle(viewPort, portWidth, portHeight, *inst.color, (int)points[inst.getTriangles()[i].a].x, (int)points[inst.getTriangles()[i].a].y,
																		   (int)points[inst.getTriangles()[i].b].x, (int)points[inst.getTriangles()[i].b].y,
																		   (int)points[inst.getTriangles()[i].c].x, (int)points[inst.getTriangles()[i].c].y);
	}
}

void Scene::wireframe(const proj::Instance &inst){
	math3d::Point2D* points = new math3d::Point2D[inst.getNumVertices()];
	for(int i = 0; i < inst.getNumVertices(); i++)
		points[i] = project(inst.getVertices()[i]);
	for(int i = 0; i < inst.getNumTriangles(); i++)
		raster::drawTriangle(viewPort, portWidth, portHeight, *inst.color, (int)points[inst.getTriangles()[i].a].x, (int)points[inst.getTriangles()[i].a].y,
																		   (int)points[inst.getTriangles()[i].b].x, (int)points[inst.getTriangles()[i].b].y,
																		   (int)points[inst.getTriangles()[i].c].x, (int)points[inst.getTriangles()[i].c].y);
}

void Scene::clear(){
	for(int i = 0; i < portWidth*portHeight; i++){
		viewPort[i] = 0xFFFFFF;
		depth[i] = 0;
	}
}

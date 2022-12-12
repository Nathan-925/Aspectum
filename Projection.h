/*
 * Projection.h
 *
 *  Created on: Dec 9, 2022
 *      Author: Nathan
 */

#ifndef PROJECTION_H_
#define PROJECTION_H_

#include <cstdint>
#include <cmath>
#include <iostream>

#include "Math3D.h"
#include "Color.h"

namespace proj{

	struct Model{
		const int numVertices;
		const int numTriangles;
		math3d::Point3D* vertices;
		int** triangles;
		math3d::Vector3D* normals;

		Model(int v, int t) : numVertices(v), numTriangles(t), vertices(new math3d::Point3D[v]), triangles(new int*[t]), normals(new math3d::Vector3D[t]) {};
	};

	class Instance{
		Model* model;

	public:
		Color* color;
		math3d::TransformationMatrix transform;

		Instance(Model &m, Color c) : model(&m), color(&c) {};
		Instance(Model* m, Color c) : model(m), color(&c) {};
		math3d::Point3D* getVertices() const;
		int** getTriangles() const;
		int getNumVertices() const;
		int getNumTriangles() const;
	};

	struct Texture{
		const int width, height;
		Color* image;
		Model* model;
		math3d::Point3D* indexes;

		Texture(Model &m, int w, int h) : width(w), height(h), image(new Color[w*h]), model(&m), indexes(new math3d::Point3D[m.numVertices]) {};
		Color getColor(double x, double y);
	};
}

struct RenderSettings{
	bool wireframe = false;
};

class Scene{
	uint32_t* viewPort;
	double* depthInverse;
	uint16_t portWidth, portHeight;
	double focalLength;
	math3d::TransformationMatrix camera;
	math3d::Plane* cullingPlanes;
	int numCullingPlanes;
	RenderSettings* settings;

	math3d::Point3D clipLine(math3d::Point3D culled, math3d::Point3D notCulled);
	void drawTriangle(Color &color, const math3d::Point3D &p1, const math3d::Point3D &p2, const math3d::Point3D &p3);
	math3d::Point3D project(const math3d::Point3D &point);

public:
	Scene(int width, int height) : viewPort(new uint32_t[width*height]), depthInverse(new double[width*height]), portWidth(width), portHeight(height), focalLength(width/2) {
		numCullingPlanes = 5;
		cullingPlanes = new math3d::Plane[numCullingPlanes];
		cullingPlanes[0] = math3d::Plane(0, 0, 1, -1);
		cullingPlanes[1] = math3d::Plane(width/2, 0, focalLength, 0);
		cullingPlanes[2] = math3d::Plane(-width/2, 0, focalLength, 0);
		cullingPlanes[3] = math3d::Plane(0, -height/2, focalLength, 0);
		cullingPlanes[4] = math3d::Plane(0, height/2, focalLength, 0);

		for(int i = 0; i < numCullingPlanes; i++){
			double mag = sqrt(cullingPlanes[i].a*cullingPlanes[i].a +
							  cullingPlanes[i].b*cullingPlanes[i].b +
							  cullingPlanes[i].c*cullingPlanes[i].c);
			cullingPlanes[i].a /= mag;
			cullingPlanes[i].b /= mag;
			cullingPlanes[i].c /= mag;
		}
		clear();
	};

	double getFocalLength(double fovDegrees);
	void render(const proj::Instance &inst);
	void wireframe(const proj::Instance &inst);
	void setRenderSettings(RenderSettings &settings) { this->settings = &settings; };
	void clear();
	uint32_t* getRaster(){ return viewPort; };
	uint16_t getWidth(){ return portWidth; };
	uint16_t getHeight(){ return portHeight; };
};

#endif /* PROJECTION_H_ */

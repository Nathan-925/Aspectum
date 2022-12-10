/*
 * Projection.h
 *
 *  Created on: Dec 9, 2022
 *      Author: Nathan
 */

#ifndef PROJECTION_H_
#define PROJECTION_H_

#include <cstdint>

#include "Math3D.h"
#include "Color.h"

namespace proj{
	struct Triangle{
		union{
			struct{
				int a, b, c;
			};
			int arr[3];
		};

		Triangle() : a(0), b(0), c(0) {};
		Triangle(int a, int b, int c) : a(a), b(b), c(c) {};
	};

	struct Model{
		math3d::Point3D* vertices;
		int numVertices;
		Triangle* triangles;
		int numTriangles;
	};

	class Instance{
		Model* model;

	public:
		Color* color;
		long double* transform = new long double[]{1, 0, 0, 0,
												   0, 1, 0, 0,
												   0, 0, 1, 0,
												   0, 0, 0, 1};

		Instance(Model m, Color c) : model(&m), color(&c) {};
		math3d::Point3D* getVertices() const;
		Triangle* getTriangles() const;
		int getNumVertices() const;
		int getNumTriangles() const;
	};
}

class Scene{
	uint32_t* viewPort;
	double* depth;
	uint16_t portWidth, portHeight;
	double focalLength;
	long double* camera;

	bool cull(math3d::Point3D point, double a, double b, double c, double d);
	math3d::Point2D project(math3d::Point3D &point);

public:
	Scene(int width, int height) : viewPort(new uint32_t[width*height]), depth(new double[width*height]), portWidth(width), portHeight(height), focalLength(width/2), camera(math3d::getUnitMatrix()) {
		clear();
	};

	double getFocalLength(double fovDegrees);
	void render(const proj::Instance &inst);
	void wireframe(const proj::Instance &inst);
	void clear();
	uint32_t* getRaster(){ return viewPort; };
	uint16_t getWidth(){ return portWidth; };
	uint16_t getHeight(){ return portHeight; };
};

#endif /* PROJECTION_H_ */

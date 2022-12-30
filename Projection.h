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

#include "priori/Math3D.h"
#include "priori/Graphical.h"

namespace proj{

	struct Model{
		const int numVertices;
		const int numTriangles;
		priori::Point3D* vertices;
		int** triangles;
		priori::Vector3D* normals;

		Model(int v, int t) : numVertices(v), numTriangles(t), vertices(new priori::Point3D[v]), triangles(new int*[t]), normals(new priori::Vector3D[t]) {};
	};

	struct Texture{
		Model* const model;
		const int width, height;
		priori::Color* image;
		double** xMap;
		double** yMap;

		Texture(Model &m, int w, int h) : model(&m), width(w), height(h), image(new priori::Color[w*h]), xMap(new double*[m.numTriangles]), yMap(new double*[m.numTriangles]) {};
		priori::Color getColor(double x, double y);
	};

	class Instance{
		Model* const model;

	public:
		priori::Color* color;
		Texture* texture;
		priori::TransformationMatrix transform;

		Instance(Model &m, priori::Color c=0xFFFFFF, Texture* t=nullptr) : model(&m), color(&c), texture(t) {};
		Instance(Model* m, priori::Color c=0xFFFFFF, Texture* t=nullptr) : model(m), color(&c), texture(t) {};
		priori::Point3D* getVertices() const;
		int** getTriangles() const;
		int getNumVertices() const;
		int getNumTriangles() const;
	};
}

struct RenderSettings{
	bool wireframe = false;
};

class Scene{
	priori::Image viewPort;
	double* depthInverse;
	double focalLength;
	priori::TransformationMatrix camera;
	priori::Plane* cullingPlanes;
	int numCullingPlanes;
	RenderSettings* settings;

	priori::Point3D clipLine(priori::Point3D culled, priori::Point3D notCulled);
	void drawTriangle(priori::Color &color, const priori::Point3D &p1, const priori::Point3D &p2, const priori::Point3D &p3);
	priori::Point3D project(const priori::Point3D &point);

public:
	Scene(int width, int height);

	double getFocalLength(double fovDegrees);
	void render(const proj::Instance &inst);
	void wireframe(const proj::Instance &inst);
	void setRenderSettings(RenderSettings &settings) { this->settings = &settings; };
	void clear();
	uint32_t* getRaster(){ return viewPort; };
};

#endif /* PROJECTION_H_ */

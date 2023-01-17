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
#include <forward_list>

#include "priori/Math3D.h"
#include "priori/Math.h"
#include "priori/Graphical.h"

namespace proj{

	struct Vertex{
		priori::Point3D position;
		priori::Point texel;
		priori::Vector3D normal;
	};

	typedef std::forward_list<Vertex[3]> Model;

	struct Texture{
		priori::Image image;

		Texture(Model &m, priori::Image image) : image(image) {};
		priori::Color getColor(double x, double y);
	};

	class Instance{
		Model* const model;

	public:
		priori::Color* color;
		Texture* texture;
		priori::TransformationMatrix transform;

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
	~Scene();

	double getFocalLength(double fovDegrees);
	void render(const proj::Instance &inst);
	void wireframe(const proj::Instance &inst);
	void setRenderSettings(RenderSettings &settings) { this->settings = &settings; };
	void clear();
	uint32_t* getRaster(){ return viewPort; };
};

#endif /* PROJECTION_H_ */

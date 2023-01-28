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

namespace asp{

	struct Texture;

	struct Vertex{
		priori::Point3D position;
		priori::Color shade;
		priori::Point texel;

		Vertex operator+(const Vertex &other);
		Vertex operator-(const Vertex &other);
		Vertex operator*(const Vertex &other);
		Vertex operator/(const Vertex &other);

		Vertex operator+=(const Vertex &other);
		Vertex operator-=(const Vertex &other);
		Vertex operator*=(const Vertex &other);
		Vertex operator/=(const Vertex &other);

		Vertex operator*(const double &d);
		Vertex operator/(const double &d);

		Vertex operator*=(const double &d);
		Vertex operator/=(const double &d);
	};

	struct Triangle{
		Vertex points[3];
		priori::Vector3D normals[3];
		Texture* texture;
		priori::Color color;

		Vertex& operator[](const int &n);
	};

	typedef std::forward_list<Triangle> Model;

	struct Texture{
		priori::Image image;

		Texture(Model &m, priori::Image image) : image(image) {};
		priori::Color getColor(double x, double y);
	};

	class Instance{
	public:
		Model* const model;
		priori::Color* color;
		Texture* texture;
		priori::TransformationMatrix transform;

		Instance(Model* m, priori::Color c=0xFFFFFF, Texture* t=nullptr) : model(m), color(&c), texture(t) {};
	};

	struct RenderSettings{
		bool wireframe = false;
		bool textures = true;
		enum{
			Flat, Gouraund, Phong
		}shading = Phong;
	};

	class Scene{
		double** depthInverse;
		priori::TransformationMatrix camera;
		double focalLength;
		std::forward_list<priori::Plane> cullingPlanes;

		Vertex clipLine(Vertex culled, Vertex notCulled);
		void drawTriangle(Triangle triangle);
		Triangle project(Triangle &triangle);
		asp::Model transformModel(asp::Model &model);
		void cull(asp::Model &triangles);
		void shadeVertices(Model &triangles);

	public:
		priori::Image viewPort;
		RenderSettings* settings;

		Scene(int width, int height);
		~Scene();

		void setFOV(double fov);
		void render(const asp::Instance &inst);
		void clear();
	};
}

#endif /* PROJECTION_H_ */

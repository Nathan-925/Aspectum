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

	class Texture;

	struct Vertex{
		priori::Point3D position;
		priori::Color shade;
		priori::Point texel;

		Vertex operator+(const Vertex &other);
		Vertex operator-(const Vertex &other);

		Vertex operator+=(const Vertex &other);
		Vertex operator-=(const Vertex &other);

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

	class Texture{
		priori::Image image;

	public:
		Texture(priori::Image image) : image(image) {};
		priori::Color getColor(double x, double y);
	};

	struct Instance{
		Model* const model;
		priori::TransformationMatrix transform;

		Instance(Model* m) : model(m) {};
	};

	struct RenderSettings{
		bool wireframe = false;
		bool textures = true;
		enum{
			Flat, Gouraund, Phong
		}shading = Phong;
	};

	struct Camera{
		priori::Point3D position;
		priori::Vector3D rotation;
	};

	class Scene{
		double** depthInverse;

		void drawTriangle(Triangle triangle);
		void project(Triangle &triangle);
		Model transformInstance(const Instance &instance);
		void cull(Model &triangles);
		void shadeVertices(Triangle &triangle);

	public:
		Camera camera;
		double focalLength;
		std::forward_list<priori::Plane> cullingPlanes;
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

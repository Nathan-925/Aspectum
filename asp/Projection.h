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

#include "LightSource.h"

#include "priori/Math3D.h"
#include "priori/Math.h"
#include "priori/Graphical.h"

namespace asp{

	class Texture;

	struct Vertex{
		priori::Point3D position;
		priori::Point texel;
		priori::Color shade;

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

	struct RenderSettings{
		bool wireframe = false;
		bool textures = true;
		enum{
			Flat, Gouraund, Phong
		}shading = Phong;
	};

	struct Camera{
		priori::Point3D position;
		double rx, ry, rz;
	};

	class Scene{
		double** depthInverse;

		void drawTriangle(Triangle triangle);
		void project(Triangle &triangle);
		Model transformModel(const Model &model, priori::TransformationMatrix transform);
		void cull(Model &triangles);
		void shadeVertices(Triangle &triangle);
		priori::Color shade(priori::Point3D point, priori::Vector3D normal);

	public:
		Camera camera;
		double focalLength;
		priori::Image viewPort;
		RenderSettings* settings;
		std::forward_list<LightSource*> lights;

		Scene(int width, int height);
		~Scene();

		void setFOV(double fov);
		void render(const Model &model, priori::TransformationMatrix transform);
		void clear();
	};
}

#endif /* PROJECTION_H_ */

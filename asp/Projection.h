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
#include <vector>

#include "Lights.h"

#include "priori/Math3D.h"
#include "priori/Math.h"
#include "priori/Graphical.h"

namespace asp{

	class Texture;

	struct Vertex{
		priori::Point3D position;
		priori::Point texel;

		Vertex operator+(Vertex other) const;
		Vertex operator-(Vertex other) const;

		Vertex operator+=(const Vertex &other);
		Vertex operator-=(const Vertex &other);

		Vertex operator*(const double &d) const;
		Vertex operator/(const double &d) const;

		Vertex operator*=(const double &d);
		Vertex operator/=(const double &d);
	};

	Vertex operator*(const priori::TransformationMatrix &transform, Vertex vertex);

	Vertex operator*=(Vertex &vertex, const priori::TransformationMatrix &transform);

	struct Fragment{
		priori::Point3D position;
		priori::Point texel;
		priori::Vector3D normal;
		priori::Color color;

		Fragment operator+(Fragment other) const;
		Fragment operator-(Fragment other) const;

		Fragment operator+=(const Fragment &other);
		Fragment operator-=(const Fragment &other);

		Fragment operator*(const double &d) const;
		Fragment operator/(const double &d) const;

		Fragment operator*=(const double &d);
		Fragment operator/=(const double &d);
	};

	struct Material{
		priori::Color ambient, diffuse, specular;
		Texture* ambientTexture = nullptr;
		Texture* diffuseTexture = nullptr;
		Texture* specularTexture = nullptr;
		double alpha;
		double shine;
		int illuminationModel;
	};

	struct Triangle{
		int vertices[3];
		priori::Vector3D normals[3];
		Material material;
	};

	struct Model{
		std::vector<Vertex> vertices;
		std::vector<Triangle> triangles;
	};

	class Texture{
		priori::Image image;

	public:
		Texture(priori::Image image) : image(image) {};
		priori::Color getColor(double x, double y);
	};

	struct RenderSettings{
		bool wireframe = false;
		bool textures = true;
		bool shading = true;
		bool specular = true;
	};

	struct Instance{
		Model* model;
		priori::TransformationMatrix transform;

		Instance(Model* m);
		Instance(Model* m, priori::TransformationMatrix t);
	};

	struct Scene{
		std::vector<Instance*> objects;
		AmbientLight ambientLight;
		std::vector<DirectionalLight*> directionalLights;
		std::vector<PointLight*> pointLights;
	};

	class Camera{
		double** depthInverse;

	public:
		double focalLength;
		priori::Image viewPort;
		RenderSettings* settings;
		priori::Point3D position;
		double rx, ry, rz;
		std::vector<void (*)(Vertex&)> vertexShaders;
		std::vector<void (*)(Fragment&)> fragmentShaders;

		Camera(int width, int height);
		~Camera();

		void setFOV(double fov);
		void render(const Scene &scene);
		void clear();
	};
}

#endif /* PROJECTION_H_ */

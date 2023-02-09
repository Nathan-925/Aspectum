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
		priori::Vector3D normal;

		Vertex operator+(const Vertex &other) const;
		Vertex operator-(const Vertex &other) const;

		Vertex operator+=(const Vertex &other);
		Vertex operator-=(const Vertex &other);

		Vertex operator*(const double &d) const;
		Vertex operator/(const double &d) const;

		Vertex operator*=(const double &d);
		Vertex operator/=(const double &d);
	};

	struct Fragment{
		int x, y;
		double depth;
		priori::Color color;
		priori::Point texel;
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
		Vertex points[3];
		Material* material = nullptr;

		Vertex& operator[](const int &n);
	};

	Triangle operator*(const priori::TransformationMatrix &transform, const Triangle &triangle);

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
		bool shading = true;
		bool specular = true;
	};

	struct Instance{
		Model* model;
		priori::TransformationMatrix transform;

		Instance(Model* m);
		Instance(Model* m, priori::TransformationMatrix t);
	};

	Instance operator*(const priori::TransformationMatrix &transform, const Instance &instance);
	Instance operator*=(Instance &instance, const priori::TransformationMatrix &transform);

	struct Scene{
		std::vector<Instance*> objects;
		AmbientLight ambientLight;
		std::vector<DirectionalLight*> directionalLights;
		std::vector<PointLight*> pointLights;
	};

	class Camera{
		double** depthInverse;

		void project(Triangle &triangle);
		priori::Color shade(priori::Point3D point, priori::Vector3D normal, Material* material);

	public:
		double focalLength;
		priori::Image viewPort;
		RenderSettings* settings;
		priori::Point3D position;
		double rx, ry, rz;

		Camera(int width, int height);
		~Camera();

		void setFOV(double fov);
		void render(const Scene &scene);
		void clear();
	};
}

#endif /* PROJECTION_H_ */

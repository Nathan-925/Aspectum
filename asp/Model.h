/*
 * Model.h
 *
 *  Created on: Mar 31, 2023
 *      Author: Nathan
 */

#ifndef MODEL_H_
#define MODEL_H_

#include <vector>
#include <string>
#include <unordered_map>

#include "priori/Math.h"
#include "priori/Math3D.h"
#include "priori/Graphical.h"

namespace asp{

class Texture;

	struct Vertex{
		priori::Vector3D position;
		priori::Vector texel;

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

	struct Material{
		priori::Color ambient, diffuse, specular;
		Texture* ambientTexture = nullptr;
		Texture* diffuseTexture = nullptr;
		Texture* specularTexture = nullptr;
		double alpha;
		double shine;
		int illuminationModel;
	};

	class Camera;
	extern priori::Image* debug;

	struct Fragment{
		Camera* camera;
		priori::Vector3D position;
		priori::Vector texel;
		priori::Vector3D normal;
		priori::Color color;
		Material material;

		Fragment operator+(Fragment other) const;
		Fragment operator-(Fragment other) const;

		Fragment operator+=(const Fragment &other);
		Fragment operator-=(const Fragment &other);

		Fragment operator*(const double &d) const;
		Fragment operator/(const double &d) const;

		Fragment operator*=(const double &d);
		Fragment operator/=(const double &d);
	};

	struct Triangle{
		int vertices[3];
		priori::Vector3D normals[3];
		Material material;
	};

	struct TextureSettings{
		bool mipmapping = true;
		bool correctGamma = true;
		enum{
			NONE, BILINEAR, TRILINEAR
		}filtering = BILINEAR;
		bool wrap = false;
	};

	class Texture{
		priori::Color bilinear(int layer, double x, double y);

		public:
		std::vector<priori::Image> images;
		TextureSettings* settings;

		Texture(priori::Image image, TextureSettings* settings);
		priori::Color shade(Fragment** fragment, int x, int y);
	};

	struct Model{
		std::vector<Vertex> vertices;
		std::vector<Triangle> triangles;
	};

	struct Instance{
		Model* model;
		priori::TransformationMatrix transform;

		Instance(Model* m);
		Instance(Model* m, priori::TransformationMatrix t);
	};

}

#endif /* MODEL_H_ */

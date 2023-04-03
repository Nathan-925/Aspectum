/*
 * Camera.h
 *
 *  Created on: Mar 31, 2023
 *      Author: Nathan
 */

#ifndef CAMERA_H_
#define CAMERA_H_

#include <vector>
#include <forward_list>

#include "Model.h"
#include "Scene.h"
#include "Shaders.h"

#include "priori/Graphical.h"

namespace asp{

	struct RenderSettings{
		bool wireframe = false;
		bool textures = true;
		bool shading = true;
		bool specular = true;
	};

	class Camera{
		double** depthInverse;

		Fragment project(const Vertex &vertex, priori::Vector3D normal, const Material &material);

	public:
		double focalLength;
		priori::Image viewPort;
		RenderSettings* settings;
		priori::Vector3D position;
		double rx, ry, rz;
		std::vector<VertexShader> vertexShaders;
		std::vector<GeometryShader> geometryShaders;
		std::vector<FragmentShader> fragmentShaders;

		Camera(int width, int height);
		~Camera();

		void setFOV(double fov);
		void render(const Scene &scene);
		void clear();
	};

}

#endif /* CAMERA_H_ */

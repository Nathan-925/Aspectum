/*
 * Camera.h
 *
 *  Created on: Mar 31, 2023
 *      Author: Nathan
 */

#ifndef CAMERA_H_
#define CAMERA_H_

#include "Model.h"
#include "Scene.h"

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

#endif /* CAMERA_H_ */

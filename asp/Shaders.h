/*
 * Shaders.h
 *
 *  Created on: Mar 29, 2023
 *      Author: Nathan
 */

#ifndef SHADERS_H_
#define SHADERS_H_

#include <vector>
#include <forward_list>

#include "Model.h"

#include "priori/Math3D.h"

namespace asp{

	typedef void (*VertexShader)(Vertex&);
	typedef void (*GeometryShader)(std::vector<Vertex>&, std::forward_list<Triangle>&);
	typedef void (*FragmentShader)(Fragment&);

	struct CullingPlane{
		priori::Plane plane;

		void cull(std::vector<Vertex> &vertices, std::forward_list<Triangle> &triangles);
	};

	void colorNormals(Fragment &fragment);

}

#endif /* SHADERS_H_ */

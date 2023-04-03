/*
 * Shaders.cpp
 *
 *  Created on: Mar 29, 2023
 *      Author: Nathan
 */
#include <iterator>

#include "Shaders.h"

using namespace std;
using namespace priori;

namespace asp{

	void CullingPlane::cull(vector<Vertex> &vertices, forward_list<Triangle> &triangles){
		auto prev = triangles.before_begin();
		for(auto it = triangles.begin(); it != triangles.end(); prev = it++){
			Triangle t = *it;
			int culled[3];
			int numCulled = 0;
			for(int i = 0; i < 3; i++){
				Vector3D pos = vertices[t.vertices[i]].position;
				if(plane.distance(pos) < 0)
					culled[numCulled++] = i;
				else
					culled[2-i+numCulled] = i;
			}

			if(numCulled == 3){
				triangles.erase_after(prev);
				it = prev;
			}
			else if(numCulled == 2){
				it->vertices[culled[0]] = vertices.size();
				vertices.push_back(
						lerp<Vertex>(plane.intersectionPercent(vertices[t.vertices[culled[0]]].position,
															   vertices[t.vertices[culled[2]]].position),
						vertices[t.vertices[culled[0]]],
						vertices[t.vertices[culled[2]]]));

				it->vertices[culled[1]] = vertices.size();
				vertices.push_back(
						lerp<Vertex>(plane.intersectionPercent(vertices[t.vertices[culled[1]]].position,
															   vertices[t.vertices[culled[2]]].position),
						vertices[t.vertices[culled[1]]],
						vertices[t.vertices[culled[2]]]));
			}
			else if(numCulled == 1){
				Triangle clipping(t);

				clipping.vertices[0] = vertices.size();
				vertices.push_back(
						lerp<Vertex>(plane.intersectionPercent(vertices[t.vertices[culled[0]]].position,
															   vertices[t.vertices[culled[1]]].position),
						vertices[t.vertices[culled[0]]],
						vertices[t.vertices[culled[1]]]));

				clipping.vertices[1] = vertices.size();
				vertices.push_back(
						lerp<Vertex>(plane.intersectionPercent(vertices[t.vertices[culled[0]]].position,
															   vertices[t.vertices[culled[2]]].position),
						vertices[t.vertices[culled[0]]],
						vertices[t.vertices[culled[2]]]));

				clipping.vertices[2] = t.vertices[culled[1]];
				it->vertices[culled[0]] = clipping.vertices[1];

				triangles.push_front(clipping);
			}
		}
	}

	void colorNormals(Fragment &fragment){
		Vector3D normal = fragment.normal.normalize()*0.5;
		fragment.color = fragment.normal.z > 0 ? 0 : Color((normal.x+0.5)*255, (normal.y+0.5)*255, (-normal.z+0.5)*255);
	}

}

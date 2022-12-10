/*
 * Shapes.cpp
 *
 *  Created on: Dec 9, 2022
 *      Author: Nathan
 */
#include "Math3D.h"
#include "Shapes.h"

proj::Model cube;

namespace shapes{
	proj::Model getCube(){
		if(cube.vertices == nullptr){
			cube.vertices = new math3d::Point3D[8];
			cube.numVertices = 8;
			for(int i = 0; i < 8; i++){
				cube.vertices[i].x = i&1;
				cube.vertices[i].y = (i>>1)&1;
				cube.vertices[i].z = (i>>2)&1;
			}
			cube.triangles = new proj::Triangle[12];
			cube.triangles[0] = proj::Triangle(0, 1, 2);
			cube.triangles[1] = proj::Triangle(3, 1, 2);
			cube.triangles[2] = proj::Triangle(0, 4, 1);
			cube.triangles[3] = proj::Triangle(5, 4, 1);
			cube.triangles[4] = proj::Triangle(1, 5, 3);
			cube.triangles[5] = proj::Triangle(7, 5, 3);
			cube.triangles[6] = proj::Triangle(3, 7, 2);
			cube.triangles[7] = proj::Triangle(6, 7, 2);
			cube.triangles[8] = proj::Triangle(2, 6, 0);
			cube.triangles[9] = proj::Triangle(4, 6, 0);
			cube.triangles[10] = proj::Triangle(4, 5, 6);
			cube.triangles[11] = proj::Triangle(7, 5, 6);

			cube.numTriangles = 12;
		}
		return cube;
	}
}

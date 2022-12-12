/*
 * Shapes.cpp
 *
 *  Created on: Dec 9, 2022
 *      Author: Nathan
 */
#include "Math3D.h"
#include "Shapes.h"

proj::Model cube(8, 12);
bool cubeInit = false;

namespace shapes{
	proj::Model& getCube(){
		if(!cubeInit){
			cubeInit = true;
			cube.vertices = new math3d::Point3D[8];
			for(int i = 0; i < 8; i++){
				cube.vertices[i].x = i&1;
				cube.vertices[i].y = (i>>1)&1;
				cube.vertices[i].z = (i>>2)&1;
			}
			cube.triangles = new int*[12];
			cube.triangles[0] = new int[]{0, 1, 2};
			cube.triangles[1] = new int[]{3, 1, 2};
			cube.triangles[2] = new int[]{0, 4, 1};
			cube.triangles[3] = new int[]{5, 4, 1};
			cube.triangles[4] = new int[]{1, 5, 3};
			cube.triangles[5] = new int[]{7, 5, 3};
			cube.triangles[6] = new int[]{3, 7, 2};
			cube.triangles[7] = new int[]{6, 7, 2};
			cube.triangles[8] = new int[]{2, 6, 0};
			cube.triangles[9] = new int[]{4, 6, 0};
			cube.triangles[10] = new int[]{4, 5, 6};
			cube.triangles[11] = new int[]{7, 5, 6};
		}
		return cube;
	}
}

/*
 * Shapes.cpp
 *
 *  Created on: Dec 9, 2022
 *      Author: Nathan
 */
#include <iostream>

#include "Shapes.h"
#include "obj/objio.h"

#include "priori/Math.h"
#include "priori/Math3D.h"

using namespace std;
using namespace priori;

namespace asp{
	Model getCube(Texture* t){

		Model cube = readobj("cube.obj");
		for(Triangle &triangle: cube.triangles){
			triangle.material = Material{
			0xFFFFFF, 0xFFFFFF, 0xFFFFFF,
			t, t, t,
			1, 0,
			1};
		}
		return cube;
	}
}

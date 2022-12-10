/*
 * Main.cpp
 *
 *  Created on: Dec 7, 2022
 *      Author: Nathan
 */
#include <iostream>
#include <cmath>

#include "Rasterizer.h"
#include "BMP.h"
#include "Math3D.h"
#include "Projection.h"
#include "Color.h"
#include "Shapes.h"
#include "Interpolation.h"

using namespace std;

int main(){
	Scene scene(1000, 1000);

	Color color;
	color.c = 0x8800FF;

	proj::Instance inst(shapes::getCube(), color);
	inst.transform = math3d::scale(inst.transform, 400, 400, 400);
	inst.transform = math3d::rotate(inst.transform, 0, M_PI/10, M_PI/10);
	inst.transform = math3d::translate(inst.transform, -250, -250, 1000);

	scene.render(inst);

	inst.color->c = 0xFF0000;
	scene.wireframe(inst);

	bmp::writeFile("line.bmp", scene.getRaster(), scene.getWidth(), scene.getHeight());
}

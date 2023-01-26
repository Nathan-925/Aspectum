/*
 * Main.cpp
 *
 *  Created on: Dec 7, 2022
 *      Author: Nathan
 */
#include <iostream>
#include <cmath>

#include "Projection.h"
#include "Shapes.h"

#include "priori/BMPLibrary.h"
#include "priori/Math3D.h"

using namespace std;
using namespace priori;
using namespace asp;

int main(){
	Triangle triangle;
	triangle[0].position = Point3D(0, 50, 100);
	triangle[1].position = Point3D(-50, -50, 100);
	triangle[2].position = Point3D(50, -50, 100);
	Model model;
	model.push_front(triangle);

	Instance instance(&model);

	Scene scene(100, 100);
	scene.focalLength = scene.getFocalLength(90);
	cout << scene.focalLength << endl;

	scene.render(instance);
}

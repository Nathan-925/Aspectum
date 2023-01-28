/*
 * Main.cpp
 *
 *  Created on: Dec 7, 2022
 *      Author: Nathan
 */
#include <iostream>
#include <cmath>
#include <forward_list>

#include "Projection.h"
#include "Shapes.h"

#include "priori/BMPLibrary.h"
#include "priori/Math3D.h"

using namespace std;
using namespace priori;
using namespace asp;

int main(){
	forward_list<int> list = lerp<int>(0, 0, 10, 20);
	for(auto it = list.begin(); it != list.end(); it++)
		cout << *it << endl;

	Triangle triangle;
	triangle.color = Color(100, 0, 200);

	triangle[0].position = Point3D(0, 25, 100);
	triangle[1].position = Point3D(0, -25, 100);
	triangle[2].position = Point3D(25, 0, 100);

	Model model;
	model.push_front(triangle);

	Instance instance(&model);

	Scene scene(100, 100);

	RenderSettings settings;
	settings.textures = false;
	settings.wireframe = false;
	scene.settings = &settings;

	scene.render(instance);
	cout << "end render" << endl;

	writebmp("test.bmp", scene.viewPort);
}

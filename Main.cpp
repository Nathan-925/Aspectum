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

	Image tex(256, 256);
	for(int i = 0; i < 256; i++)
		for(int j = 0; j < 256; j++)
			tex[i][j] = Color(i, 0, j);
	Texture colorTexture(tex);

	Image noe = readbmp("noe.bmp");
	Texture noeTexture(noe);

	Vertex vertices[4];

	vertices[0].position = Point3D(-30, -30, 80);
	vertices[0].texel = Point(0, 0);

	vertices[1].position = Point3D(30, -30, 40);
	vertices[1].texel = Point(1, 0);

	vertices[2].position = Point3D(-30, 30, 80);
	vertices[2].texel = Point(0, 1);

	vertices[3].position = Point3D(30, 30, 40);
	vertices[3].texel = Point(1, 1);

	Triangle t1, t2;

	t1.color = 0xFF0000;
	t1.texture = &noeTexture;
	t1.points[0] = vertices[0];
	t1.points[1] = vertices[1];
	t1.points[2] = vertices[2];

	t2.color = 0xFF;
	t2.texture = &noeTexture;
	t2.points[0] = vertices[3];
	t2.points[1] = vertices[1];
	t2.points[2] = vertices[2];

	Model model = getCube(&noeTexture);

	Instance instance(&model);
	instance.transform *= scale(50, 50, 50);
	instance.transform *= rotateX(M_PI/3);
	instance.transform *= rotateY(M_PI/8);
	instance.transform *= translate(60, 0, 100);

	Scene scene(500, 500);

	RenderSettings settings;
	settings.textures = true;
	settings.wireframe = true;
	scene.settings = &settings;

	for(int i = 0; i < 4; i++){
		scene.render(instance);
		instance.transform *= rotateZ(M_PI/2);
	}
	cout << "end render" << endl;

	writebmp("test.bmp", scene.viewPort);
	cout << "end program" << endl;
}

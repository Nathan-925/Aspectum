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
#include "obj/objio.h"

#include "priori/BMPLibrary.h"
#include "priori/Math3D.h"

using namespace std;
using namespace priori;
using namespace asp;

int main(){

	Model testModel = readobj("test.obj");
	for(Triangle t: testModel){
		for(int i = 0; i < 3; i++){
			cout << "position " << t[i].position.x << " " << t[i].position.y << " " << t[i].position.z << endl;
			cout << "texel " << t[i].texel.x << " " << t[i].texel.y << endl;
			cout << "normal " << t.normals[i].x << " " << t.normals[i].y << " " << t.normals[i].z << endl;
		}
	}

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

	Model model = readobj("untitled.obj");

	//model.emplace_front();
	//model.front().texture = &noeTexture;
	//model.front()[0].position = Point3D(0, 0, 0);
	//model.front()[1].position = Point3D(1, 0, 0);
	//model.front()[2].position = Point3D(0, 1, 0);
	//model.front()[0].texel = Point(0, 1);
	//model.front()[1].texel = Point(1, 1);
	//model.front()[2].texel = Point(0, 0);
	//model.front().normals[0] = (model.front()[2].position-model.front()[1].position)^(model.front()[0].position-model.front()[1].position);
	//model.front().normals[1] = model.front().normals[0];
	//model.front().normals[2] = model.front().normals[0];

	Scene scene;
	Instance i(&model);
	i *= scale(30, 30, 30);
	//i *= rotateX(M_PI/3);
	i *= rotateY(M_PI/8);
	i *= translate(0, -2500, 5000);
	scene.objects.push_back(&i);

	Camera camera(1000, 1000);

	//camera.camera.position = Point3D(0, 0, 0);
	//camera.camera.ry = -M_PI/2;

	RenderSettings settings;
	settings.textures = true;
	settings.wireframe = true;
	settings.shadingMode = settings.Gouraund;
	camera.settings = &settings;

	camera.render(scene);
	cout << "end render" << endl;

	writebmp("test.bmp", camera.viewPort);
	cout << "end program" << endl;
}

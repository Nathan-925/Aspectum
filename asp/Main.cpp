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

	Model model;

	model.emplace_front();
	model.front().texture = &noeTexture;
	model.front()[0].position = Point3D(0, 0, 0);
	model.front()[1].position = Point3D(1, 0, 0);
	model.front()[2].position = Point3D(0, 1, 0);
	model.front()[0].texel = Point(0, 1);
	model.front()[1].texel = Point(1, 1);
	model.front()[2].texel = Point(0, 0);
	model.front().normals[0] = (model.front()[2].position-model.front()[1].position)^(model.front()[0].position-model.front()[1].position);
	model.front().normals[1] = model.front().normals[0];
	model.front().normals[2] = model.front().normals[0];

	TransformationMatrix instance;
	instance *= scale(50, 50, 50);
	//instance.transform *= rotateY(M_PI);
	instance *= translate(0, 0, 100);

	Scene scene(1000, 1000);
	scene.lights.push_front(new DirectionalLight(0xFFFFFF, 1, Vector3D(0, 0, 1)));

	//scene.camera.position = Point3D(0, 0, 0);
	//scene.camera.ry = -M_PI/2;

	RenderSettings settings;
	settings.textures = true;
	settings.wireframe = false;
	settings.shading = settings.Gouraund;
	scene.settings = &settings;

	scene.render(model, instance);
	cout << "end render" << endl;

	writebmp("test.bmp", scene.viewPort);
	cout << "end program" << endl;
}

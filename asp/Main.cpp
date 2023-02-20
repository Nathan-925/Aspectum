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

	Texture noeTexture(readbmp("noe.bmp"));

	Model model = readobj("untitled.obj");

	Scene scene;

	scene.ambientLight.color = 0xFFFFFF;
	scene.ambientLight.intensity = 0.5;

	//DirectionalLight d1{0xFFFFFF, 0.5, Vector3D(1, -1, 1)};
	//scene.directionalLights.push_back(&d1);

	//PointLight p{0xFFFFFF, 1, Point3D(0, 0, 0)};
	//scene.pointLights.push_back(&p);

	Instance i(&model);
	//i.transform *= scale(10, 10, 10);
	i.transform *= rotateY(M_PI);
	i.transform *= translate(0, -100, 200);
	scene.objects.push_back(&i);

	Camera camera(1000, 1000);

	//camera.camera.position = Point3D(0, 0, 0);
	//camera.camera.ry = -M_PI/2;

	RenderSettings settings;
	settings.wireframe = false;
	settings.textures = false;
	settings.shading = true;
	settings.specular = true;
	camera.settings = &settings;

	camera.render(scene);
	cout << "end render" << endl;

	//for(int i = 0; i < camera.viewPort.width; i++)
	//	camera.viewPort[i][camera.viewPort.height/2] = 0xFF0000;
	//for(int i = 0; i < camera.viewPort.height; i++)
	//	camera.viewPort[camera.viewPort.width/2][i] = 0xFF0000;

	writebmp("test.bmp", camera.viewPort);
	cout << "end program" << endl;
}

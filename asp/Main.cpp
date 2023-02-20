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

	Model model = readobj("sphere.obj");
	for(Triangle &t: model){
		t.material.illuminationModel = 2;
		t.material.ambient = 0xFFFFFF;
		t.material.diffuse = 0;
		t.material.specular = 0xFFFFFF;
		t.material.shine = 100;
	}

	Scene scene;

	//scene.ambientLight.color = 0xFFFFFF;
	//scene.ambientLight.intensity = 0.2;

	DirectionalLight d1{0xFF0000, 1, Vector3D(1, 0, 0)};
	scene.directionalLights.push_back(&d1);
	//DirectionalLight d2{0xFF00, 1, Vector3D(-1, 0, 0)};
	//scene.directionalLights.push_back(&d2);
	//DirectionalLight d3{0xFF, 1, Vector3D(0, 1, 0)};
	//scene.directionalLights.push_back(&d3);

	//PointLight p{0xFFFFFF, 1, Point3D(0, 0, 0)};
	//scene.pointLights.push_back(&p);

	Instance i(&model);
	i.transform *= scale(10, 10, 10);
	//i.transform *= rotateX(M_PI/4);
	//i.transform *= rotateY(M_PI/4);
	i.transform *= translate(0, 0, 100);
	scene.objects.push_back(&i);

	Camera camera(1000, 1000);

	//camera.camera.position = Point3D(0, 0, 0);
	//camera.camera.ry = -M_PI/2;

	RenderSettings settings;
	settings.wireframe = false;
	settings.textures = true;
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

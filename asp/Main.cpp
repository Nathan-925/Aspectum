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

	for(Triangle &t: model.triangles){
		t.material.illuminationModel = 2;
		t.material.ambient = 0xFFFFFF;
		t.material.diffuse = 0xFFFFFF;
		t.material.specular = 0xFFFFFF;
		t.material.shine = 10;
	}

	Scene scene;

	scene.ambientLight.color = 0xFFFFFF;
	scene.ambientLight.intensity = 0.3;

	DirectionalLight d1{0xFF, 0.5, Vector3D(1, -1, 1)};
	scene.directionalLights.push_back(&d1);

	PointLight p{0xFF00, 0.5, Point3D(25, 0, 25)};
	scene.pointLights.push_back(&p);

	Instance i(&model);
	i.transform *= scale(10, 10, 10);
	i.transform *= translate(0, 0, 50);
	scene.objects.push_back(&i);

	Camera camera(1000, 1000);

	//camera.position = Point3D(-50, 0, 0);

	RenderSettings settings;
	settings.wireframe = false;
	settings.textures = true;
	settings.shading = true;
	settings.specular = true;
	camera.settings = &settings;

	camera.render(scene);
	cout << "end render" << endl;

	writebmp("test.bmp", camera.viewPort);
	cout << "end program" << endl;
}

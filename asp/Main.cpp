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
#include "Shaders.h"
#include "obj/objio.h"

#include "priori/BMPLibrary.h"
#include "priori/Math3D.h"

using namespace std;
using namespace priori;
using namespace asp;

int main(){

	Texture noeTexture(readbmp("noe.bmp"));
	Texture skeeterTexture(readbmp("skeeter.bmp"));

	//Model model = readobj("cube.obj");
	Model model;
	model.vertices.emplace_back();
	model.vertices.back().position = Point3D(0, 0, 0);
	model.vertices.emplace_back();
	model.vertices.back().position = Point3D(1, 0, 0);
	model.vertices.emplace_back();
	model.vertices.back().position = Point3D(0, 1, 0);

	for(Triangle &t: model.triangles){
		t.material.illuminationModel = 2;
		t.material.ambient = 0xFFFFFF;
		t.material.diffuse = 0;
		t.material.specular = 0xFFFFFF;
		t.material.shine = 10;
		t.material.ambientTexture = &skeeterTexture;
		t.material.diffuseTexture = &skeeterTexture;
		t.material.specularTexture = &skeeterTexture;
	}

	model.triangles.emplace_back();
	model.triangles.back().vertices[0] = 0;
	model.triangles.back().vertices[1] = 1;
	model.triangles.back().vertices[2] = 2;
	model.triangles.back().material.diffuse = 0;

	Scene scene;

	scene.ambientLight.color = 0xFFFFFF;
	scene.ambientLight.intensity = 0.3;

	DirectionalLight d1{0xFFFFFF, 0.7, Vector3D(0, 0, 1)};
	scene.directionalLights.push_back(&d1);

	PointLight p{0xFFFF00, 0.5, Point3D(25, 0, 0)};
	//scene.pointLights.push_back(&p);

	Instance i(&model);
	//i.transform *= translate(0, -1, 0);
	i.transform *= scale(10, 100, 10);
	//i.transform *= rotateZ(M_PI/2);
	//i.transform *= rotateY(M_PI/4);
	//i.transform *= rotateZ(M_PI/4);
	i.transform *= translate(0, 0, 30);
	scene.objects.push_back(&i);

	Camera camera(1000, 1000);
	camera.fragmentShaders.push_back(colorNormals);

	//camera.position = Point3D(-50, 0, 0);

	RenderSettings settings;
	settings.wireframe = true;
	settings.textures = false;
	settings.shading = false;
	settings.specular = false;
	camera.settings = &settings;

	camera.render(scene);
	cout << "end render" << endl;

	settings.wireframe = true;
	camera.render(scene);

	writebmp("test.bmp", camera.viewPort);
	cout << "end program" << endl;
}

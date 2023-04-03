/*
 * Main.cpp
 *
 *  Created on: Dec 7, 2022
 *      Author: Nathan
 */
#include <iostream>
#include <cmath>
#include <forward_list>
#include <numbers>

#include "Scene.h"
#include "Model.h"
#include "Camera.h"
#include "Lights.h"
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

	//Model model = readobj("mario/untitled.obj");
	Model model;
	model.vertices.emplace_back();
	model.vertices.back().position = Vector3D{0, 0, 0, true};
	model.vertices.emplace_back();
	model.vertices.back().position = Vector3D{1, 0, 0, true};
	model.vertices.emplace_back();
	model.vertices.back().position = Vector3D{0, 1, 0, true};

	//for(Triangle &t: model.triangles){
	//	t.material.illuminationModel = 2;
	//	t.material.ambient = t.material.diffuse;
	//	//t.material.diffuse = 0xFFFFFF;
	//	t.material.specular = t.material.diffuse;
	//	t.material.shine = 10;
	//	//t.material.ambientTexture = &noeTexture;
	//	//t.material.diffuseTexture = &noeTexture;
	//	//t.material.specularTexture = &noeTexture;
	//}

	model.triangles.emplace_back();
	model.triangles.back().vertices[0] = 0;
	model.triangles.back().vertices[1] = 1;
	model.triangles.back().vertices[2] = 2;
	model.triangles.back().normals[0] = Vector3D{0, 0, -1};
	model.triangles.back().normals[1] = Vector3D{0, 0, -1};
	model.triangles.back().normals[2] = Vector3D{0, 0, -1};
	model.triangles.back().material.diffuse = 0;

	Scene scene;

	Light aLight;
	aLight.color = 0xFFFFFF;
	aLight.intensity = 0.3;
	scene.lights.push_back(&aLight);

	DirectionalLight dLight;
	dLight.color = 0x5500FF;
	dLight.intensity = 0.3;
	dLight.vector = Vector3D{1, -1, 1};
	scene.lights.push_back(&dLight);

	PointLight pLight;
	pLight.color = 0xFFFF00;
	pLight.intensity = 0.3;
	pLight.point = Vector3D{20, -20, 0, true};
	scene.lights.push_back(&pLight);

	Instance i(&model);
	//i.transform *= translate(0, -70, 0);
	i.transform *= scale(10, 10, 10);
	//i.transform *= rotateX(M_PI/6);
	//i.transform *= rotateY(numbers::pi*7/6);
	//i.transform *= rotateZ(M_PI/6);
	i.transform *= translate(0, 0, 30);
	scene.objects.push_back(&i);

	Camera camera(1000, 1000);
	//camera.fragmentShaders.push_back(colorNormals);

	//camera.position = Point3D(-50, 0, 0);

	RenderSettings settings;
	settings.wireframe = true;
	settings.textures = true;
	settings.shading = true;
	settings.specular = true;
	camera.settings = &settings;

	camera.render(scene);
	cout << "end render" << endl;

	settings.wireframe = true;
	//camera.render(scene);

	writebmp("test.bmp", camera.viewPort);
	cout << "end program" << endl;
}

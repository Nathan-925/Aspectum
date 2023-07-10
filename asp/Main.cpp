/*
 * Main.cpp
 *
 *  Created on: Dec 7, 2022
 *      Author: Nathan
 */
#include <iostream>
#include <cmath>
#include <forward_list>

#include "Scene.h"
#include "Model.h"
#include "Camera.h"
#include "Lights.h"
#include "Shaders.h"
#include "ResourceLoader.h"

#include "priori/BMPLibrary.h"
#include "priori/Math3D.h"
#include "priori/Math.h"

using namespace std;
using namespace priori;
using namespace asp;

int main(){

	Fragment f1, f2;
	f1.texel = Vector{0, 0};
	f2.texel = Vector{10, -20};
	Fragment* arr = lerp<Fragment>(0, f2, 10, f1, 11, -1);
	for(int i = 0; i <= 10; i++)
		cout << arr[i].texel.x << "," << arr[i].texel.y << " ";
	cout << endl;

	Color c1(0), c2(0xFFFFFF);
	cout << hex << (uint32_t)lerp<Color>(0.3, c1, c2) << " " << (uint32_t)lerp<Color>(0.3, c2, c1) << dec << endl;

	RenderSettings settings;
	settings.wireframe = false;
	settings.backFaceCulling = false;
	settings.textures = true;
	settings.textureSettings.mipmapping = true;
	settings.textureSettings.filtering = settings.textureSettings.BILINEAR;
	settings.shading = false;
	settings.specular = true;

	Color c(0.991102*255, 0.533276*255, 0.191202*255);
	cout << (int)c.r << " " << (int)c.g << " " << (int)c.b << endl;

	ResourceLoader loader;
	loader.textureSettings = &settings.textureSettings;
	//loader.textures.emplace("noe", readbmp("noe.bmp"));
	//loader.textures.emplace("skeeter", readbmp("skeeter.bmp"));


	loader.readobj("mario/untitled.obj");
	loader.readobj("cube/cube.obj");
	//Model* model = &loader.models["cube/cube.obj"];
	Model model;
	model.vertices.emplace_back();
	model.vertices.back().position = Vector3D{0, 0, 0, true};
	model.vertices.emplace_back();
	model.vertices.back().position = Vector3D{0, 1, 0, true};
	model.vertices.emplace_back();
	model.vertices.back().position = Vector3D{1, 0, 0, true};
	model.vertices.emplace_back();
	model.vertices.back().position = Vector3D{1, 1, 0, true};

	//cout << hex << (uint32_t)model.materials["mario_face"].diffuse << " " << (uint32_t)model.materials["mario_eyes"].diffuseTexture->getColor(0, 0) << dec << endl;

	//for(Triangle &t: model->triangles){
	//	t.material.illuminationModel = 2;
	//	//if(t.material.diffuseTexture != nullptr){
	//	//	t.material.diffuse = 0xFF00FF;
	//	//	t.material.diffuseTexture = nullptr;
	//	//}
	//	t.material.ambient = t.material.diffuse;
	//	t.material.specular = t.material.diffuse;
	//	t.material.shine = 10;
	//	t.material.ambientTexture = t.material.diffuseTexture;
	//	//t.material.diffuseTexture = &skeeterTexture;
	//	t.material.specularTexture = t.material.diffuseTexture;
	//}

	model.triangles.emplace_back();
	model.triangles.back().vertices[0] = 0;
	model.triangles.back().vertices[1] = 1;
	model.triangles.back().vertices[2] = 2;
	model.triangles.back().normals[0] = Vector3D{0, 0, -1};
	model.triangles.back().normals[1] = Vector3D{0, 0, -1};
	model.triangles.back().normals[2] = Vector3D{0, 0, -1};
	model.triangles.back().texels[0] = Vector{0, 0};
	model.triangles.back().texels[1] = Vector{0, 1};
	model.triangles.back().texels[2] = Vector{1, 0};
	model.triangles.back().material.diffuse = 0xFFFFFF;
	model.triangles.back().material.alpha = 0.6;
	//model.triangles.back().material.diffuseTexture = &loader.textures.at("t.bmp");
	Texture skeeterTexture = Texture(readbmp("skeeter.bmp"), &settings.textureSettings);
	model.triangles.back().material.diffuseTexture = &skeeterTexture;
			//new Texture(readbmp("skeeter.bmp"), &settings.textureSettings);

	model.triangles.emplace_back();
	model.triangles.back().vertices[0] = 3;
	model.triangles.back().vertices[1] = 2;
	model.triangles.back().vertices[2] = 1;
	model.triangles.back().normals[0] = Vector3D{0, 0, -1};
	model.triangles.back().normals[1] = Vector3D{0, 0, -1};
	model.triangles.back().normals[2] = Vector3D{0, 0, -1};
	model.triangles.back().texels[0] = Vector{1, 1};
	model.triangles.back().texels[1] = Vector{1, 0};
	model.triangles.back().texels[2] = Vector{0, 1};
	model.triangles.back().material.diffuse = 0xFFFFFF;
	model.triangles.back().material.alpha = 0.6;

	model.triangles.back().material.diffuseTexture = model.triangles.front().material.diffuseTexture;

	Scene scene;

	//Instance i(&model);
	////i.transform *= rotateZ(-M_PI/6);
	//i.transform *= translate(-0.1, -0.1, 2);
	////i.transform *= rotateZ(M_PI/6);
	//i.transform *= scale(2, 2, 1);
	//scene.objects.push_back(&i);

	for(int w = 0; w < 60; w++)
		for(int i = 0; i < 120; i++){
			Instance* inst = new Instance(&model);
			inst->transform *= rotateX(-M_PI/2);
			inst->transform *= scale(4, 4, 4);
			inst->transform *= translate(4*w-122, -2, 4*i+2);
			scene.objects.push_back(inst);
		}

	//for(int i = 0; i < 60; i++){
	//	Instance* inst = new Instance(&model);
	//	inst->transform *= rotateX(-M_PI/2);
	//	inst->transform *= scale(4, 4, 4);
	//	inst->transform *= translate(-2, -2, 4*i+2);
	//	scene.objects.push_back(inst);
	//}

	Light aLight;
	aLight.color = 0xFFFFFF;
	aLight.intensity = 0.3;
	scene.lights.push_back(&aLight);

	DirectionalLight dLight;
	dLight.color = 0xFFFFFF;
	dLight.intensity = 0.3;
	dLight.vector = Vector3D{0, 0, 1};
	scene.lights.push_back(&dLight);

	PointLight pLight;
	pLight.color = 0xFFFFFF;
	pLight.intensity = 0.3;
	pLight.point = Vector3D{20, -20, 0, true};
	scene.lights.push_back(&pLight);

	//Instance i(model);
	////i.transform *= translate(0, -75, 0);
	//i.transform *= scale(10, 10, 100);
	////i.transform *= rotateX(-M_PI/8);
	////i.transform *= rotateY(M_PI*7/6);
	////i.transform *= rotateZ(M_PI/8);
	//i.transform *= translate(0, -10, 50);
	//scene.objects.push_back(&i);

	//for(int i = 0; i < 60; i++){
	//	Instance* inst = new Instance(&model);
	//	scene.objects.push_back(inst);
	//	inst->transform *= scale(5, 5, 5);
	//	inst->transform *= rotateX(M_PI/4);
	//	inst->transform *= translate(0, -5, 5*i);
	//}

	Camera camera(1000, 1000);
	camera.settings = &settings;
	//camera.fragmentShaders.push_back(colorNormals);

	//camera.position = Vector3D{-70, 150, 70, true};
	//camera.ry = M_PI/4;
	//camera.rx = M_PI/6;

	camera.render(scene);
	cout << "end render" << endl;

	settings.wireframe = true;
	//for(Triangle &t: model.triangles){
	//	t.material.diffuse = 0xFF00FF;
	//	t.material.diffuseTexture = nullptr;
	//}
	//camera.render(scene);

	writebmp("test.bmp", camera.viewPort);
	cout << "end program" << endl;
}

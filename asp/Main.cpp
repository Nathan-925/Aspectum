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

	RenderSettings settings;
	settings.wireframe = false;
	settings.backFaceCulling = false;
	settings.textures = true;
	settings.textureSettings.mipmapping = false;
	settings.textureSettings.filtering = settings.textureSettings.BILINEAR;
	settings.shading = true;
	settings.specular = true;

	ResourceLoader loader;
	loader.textureSettings = &settings.textureSettings;
	loader.addTexture("noe", readbmp("noe.bmp"));
	loader.addTexture("skeeter", readbmp("skeeter.bmp"));
	loader.addTexture("wood", readbmp("wood.bmp"));


	loader.readobj("mario/untitled.obj");
	loader.readobj("cube/cube.obj");
	Model* model = &loader.models["mario/untitled.obj"];
	//Model model;
	//model.vertices.emplace_back();
	//model.vertices.back().position = Vector3D{0, 0, 0, true};
	//model.vertices.emplace_back();
	//model.vertices.back().position = Vector3D{0, 1, 0, true};
	//model.vertices.emplace_back();
	//model.vertices.back().position = Vector3D{1, 0, 0, true};
	//model.vertices.emplace_back();
	//model.vertices.back().position = Vector3D{1, 1, 0, true};

	//model.triangles.emplace_back();
	//model.triangles.back().vertices[0] = 0;
	//model.triangles.back().vertices[1] = 1;
	//model.triangles.back().vertices[2] = 2;
	//model.triangles.back().normals[0] = Vector3D{0, 0, -1};
	//model.triangles.back().normals[1] = Vector3D{0, 0, -1};
	//model.triangles.back().normals[2] = Vector3D{0, 0, -1};
	//model.triangles.back().texels[0] = Vector{0, 0};
	//model.triangles.back().texels[1] = Vector{0, 1};
	//model.triangles.back().texels[2] = Vector{1, 0};
	//model.triangles.back().material.diffuse = 0xFFFFFF;
	//model.triangles.back().material.alpha = 0.6;
	////model.triangles.back().material.diffuseTexture = &loader.textures.at("t.bmp");
	////model.triangles.back().material.diffuseTexture = &loader.textures.at("skeeter");
	//model.triangles.back().material.diffuseTexture = &loader.textures.at("wood");
	//		//new Texture(readbmp("skeeter.bmp"), &settings.textureSettings);
    //
	//model.triangles.emplace_back();
	//model.triangles.back().vertices[0] = 3;
	//model.triangles.back().vertices[1] = 2;
	//model.triangles.back().vertices[2] = 1;
	//model.triangles.back().normals[0] = Vector3D{0, 0, -1};
	//model.triangles.back().normals[1] = Vector3D{0, 0, -1};
	//model.triangles.back().normals[2] = Vector3D{0, 0, -1};
	//model.triangles.back().texels[0] = Vector{1, 1};
	//model.triangles.back().texels[1] = Vector{1, 0};
	//model.triangles.back().texels[2] = Vector{0, 1};
	//model.triangles.back().material.diffuse = 0xFFFFFF;
	//model.triangles.back().material.alpha = 0.6;
    //
	//model.triangles.back().material.diffuseTexture = model.triangles.front().material.diffuseTexture;

	Scene scene;

	//Instance i(&model);
	//i.transform *= rotateX(-M_PI/2);
	//i.transform *= translate(0, 0, 0);
	////i.transform *= rotateZ(M_PI/6);
	//i.transform *= scale(2, 2, 1);
	//scene.objects.push_back(&i);

	//for(int w = 0; w < 30; w++)
	//	for(int i = 0; i < 60; i++){
	//		Instance* inst = new Instance(&model);
	//		inst->transform *= rotateX(-M_PI/2);
	//		inst->transform *= scale(10, 10, 10);
	//		inst->transform *= translate(10*w-155, -5, 10*i+7);
	//		scene.objects.push_back(inst);
	//	}

	//for(int i = 0; i < 60; i++){
	//	Instance* inst = new Instance(&model);
	//	inst->transform *= rotateX(-M_PI/2);
	//	inst->transform *= scale(4, 4, 4);
	//	inst->transform *= translate(-2, -2, 4*i+2);
	//	scene.objects.push_back(inst);
	//}

	Instance i(model);
	i.transform *= rotateY(M_PI*5/4);
	i.transform *= translate(0, -75, 125);
	scene.objects.push_back(&i);

	Light aLight;
	aLight.color = 0xFFFFFF;
	aLight.intensity = 0.2;
	scene.lights.push_back(&aLight);

	DirectionalLight dLight;
	dLight.color = 0xFFFFFF;
	dLight.intensity = 0.5;
	dLight.vector = Vector3D{-1, 0, 2};
	scene.lights.push_back(&dLight);

	PointLight pLight;
	pLight.color = 0xFFFFFF;
	pLight.intensity = 0.2;
	pLight.point = Vector3D{-50, 50, 0, true};
	scene.lights.push_back(&pLight);

	Camera camera(1000, 1000);
	camera.settings = &settings;
	//camera.fragmentShaders.push_back(colorNormals);

	//camera.position = Vector3D{-70, 150, 70, true};
	//camera.ry = M_PI/4;
	//camera.rx = M_PI/6;

	camera.render(scene);
	cout << "end render" << endl;

	//settings.wireframe = true;
	//for(Triangle &t: model->triangles){
	//	t.material.diffuse = 0;
	//}
	//camera.render(scene);

	writebmp("test.bmp", camera.viewPort);
	cout << "end program" << endl;
}

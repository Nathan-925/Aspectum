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

	Model model = getCube(&noeTexture);
	for(Triangle t: model)
		cout << t.material.illuminationModel << endl;

	Scene scene;

	DirectionalLight d{0xFFFFFF, 1, Vector3D(0, 0, 1)};
	scene.directionalLights.push_back(&d);

	Instance i(&model);
	i.transform *= scale(30, 30, 30);
	//i *= rotateX(M_PI/3);
	i.transform *= rotateY(M_PI/8);
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

	writebmp("test.bmp", camera.viewPort);
	cout << "end program" << endl;
}

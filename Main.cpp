/*
 * Main.cpp
 *
 *  Created on: Dec 7, 2022
 *      Author: Nathan
 */
#include <iostream>
#include <cmath>

#include "Rasterizer.h"
#include "Math3D.h"
#include "Projection.h"
#include "Color.h"
#include "Shapes.h"
#include "Interpolation.h"

using namespace std;

int main(){
	//srand(time(0));
	srand(24);

	Scene scene(250, 250);

	proj::Model cube = shapes::getCube();
	proj::Model model(3, 1);
	model.vertices = new math3d::Point3D[model.numVertices];
	model.vertices[0] = math3d::Point3D(0, 0, 0);
	model.vertices[1] = math3d::Point3D(-50, 50, 0);
	model.vertices[2] = math3d::Point3D(50, 50, 0);

	model.triangles = new int*[model.numTriangles];
	model.triangles[0] = new int[]{0, 1, 2};

	proj::Texture texture(cube, 256, 256);
	for(int i = 0; i < 256; i++){
		for(int j = 0; j < 256; j++){
			texture.image[i+256*j] = Color(i, 0, j);
		}
	}

	for(int i = 0; i < 12; i+=2){
		texture.xMap[i] = new double[]{0, 1, 0};
		texture.yMap[i] = new double[]{0, 0, 1};

		texture.xMap[i] = new double[]{1, 1, 0};
		texture.yMap[i] = new double[]{1, 0, 1};
	}

	Color color(0);

	proj::Instance inst1(cube, color);
	inst1.transform = math3d::translate(inst1.transform, -0.5, -0.5, -0.5);
	inst1.transform = math3d::scale(inst1.transform, 25, 25, 25);
	inst1.transform = math3d::rotateX(inst1.transform, M_PI/4);
	inst1.transform = math3d::translate(inst1.transform, 0, 0, 50);

	proj::Instance inst2(model, color);
	inst2.transform = math3d::scale(inst2.transform, 400, 800, 1);
	inst2.transform = math3d::translate(inst2.transform, 0, -400, 1000);

	proj::Instance inst3(cube, color);
	inst3.transform = math3d::translate(inst3.transform, -0.5, -0.5, -0.5);
	inst3.transform = math3d::scale(inst3.transform, 250, 250, 250);
	inst3.transform = math3d::rotateY(inst3.transform, M_PI/4);
	inst3.transform = math3d::rotateX(inst3.transform, M_PI/6);
	inst3.transform = math3d::translate(inst3.transform, 0, 0, 250);

	RenderSettings settings;
	//settings.wireframe = true;
	scene.setRenderSettings(settings);

	//scene.render(inst1);
	//scene.render(inst2);
	scene.render(inst3);

	//cout << "render" << endl;
	bmp::writeFile("line.bmp", scene.getRaster(), scene.getWidth(), scene.getHeight());
}

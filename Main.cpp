/*
 * Main.cpp
 *
 *  Created on: Dec 7, 2022
 *      Author: Nathan
 */
#include <iostream>
#include <cmath>

#include "Rasterizer.h"
#include "BMP.h"
#include "Math3D.h"
#include "Projection.h"
#include "Color.h"
#include "Shapes.h"
#include "Interpolation.h"

using namespace std;

int main(){
	Scene scene(1000, 1000);

	proj::Model model(3, 1);
	model.vertices = new math3d::Point3D[model.numVertices];
	model.vertices[0] = math3d::Point3D(0, 0, 0);
	model.vertices[1] = math3d::Point3D(-50, 50, 0);
	model.vertices[2] = math3d::Point3D(50, 50, 0);

	model.triangles = new int*[model.numTriangles];
	model.triangles[0] = new int[]{0, 1, 2};

	proj::Texture texture(shapes::getCube(), 256, 256);
	for(int i = 0; i < 256; i++){
		for(int j = 0; j < 256; j++){
			texture.image[i+texture.width*j] = Color(i, 0, j);
		}
	}



	Color color(0);

	proj::Instance inst1(shapes::getCube(), color);
	inst1.transform = math3d::translate(inst1.transform, -0.5, -0.5, -0.5);
	inst1.transform = math3d::scale(inst1.transform, 100, 100, 100);
	inst1.transform = math3d::rotate(inst1.transform, M_PI/4, M_PI/4, 0);
	inst1.transform = math3d::translate(inst1.transform, 0, 0, 250);

	proj::Instance inst2(model, color);
	inst2.transform = math3d::scale(inst2.transform, 400, 800, 1);
	inst2.transform = math3d::translate(inst2.transform, 0, -400, 1000);

	proj::Instance inst3(shapes::getCube(), color);
	inst3.transform = math3d::translate(inst3.transform, -0.5, -0.5, -0.5);
	inst3.transform = math3d::scale(inst3.transform, 250, 250, 250);
	inst3.transform = math3d::rotate(inst3.transform, 0, M_PI/4, M_PI/3);
	inst3.transform = math3d::translate(inst3.transform, 150, -150, 500);

	RenderSettings settings;
	//settings.wireframe = true;
	scene.setRenderSettings(settings);

	cout << "here1" << endl;
	scene.render(inst1);
	//scene.render(inst2);
	//scene.render(inst3);

	cout << "here2" << endl;
	bmp::writeFile("line.bmp", scene.getRaster(), scene.getWidth(), scene.getHeight());
}

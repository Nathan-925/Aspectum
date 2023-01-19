/*
 * Rasterizer.cpp
 *
 *  Created on: Dec 4, 2022
 *      Author: Nathan
 */
#include <cmath>

#include "Rasterizer.h"
#include "priori/Math.h"
#include "priori/Graphical.h"

using namespace std;
using namespace priori;

void raster::drawLine(Image target, Color color, Point p1, Point p2){
	int dx = abs(p2.x-p1.x);
	int dy = abs(p2.y-p1.y);
	if(dx > dy){
		long double* line = priori::lerp<double>(p1.x, p1.y, p2.x, p2.y);
		for(int i = 0; i <= dx; i++)
			target.pixels[i+p1.x][line[i]] = color;
	}
	else{
		long double* line = priori::lerp<double>(p1.y, p1.x, p2.y, p2.x);
		for(int i = 0; i <= dy; i++)
			target.pixels[line[i]][i+p1.y] = color;
	}
}

void raster::drawTriangle(Image target, Color color, Point p1, Point p2, Point p3){
	raster::drawLine(target, color, p1, p2);
	raster::drawLine(target, color, p1, p3);
	raster::drawLine(target, color, p2, p3);
}

void raster::fillTriangle(Image target, Color color, Point p1, Point p2, Point p3){
	if(p2.y < p1.y){
		Point temp = p1;
		p1 = p2;
		p2 = temp;
	}
	if(p3.y < p1.y){
		Point temp = p1;
		p1 = p3;
		p3 = temp;
	}
	if(p3.y < p2.y){
		Point temp = p2;
		p2 = p3;
		p3 = temp;
	}

	int dy01 = abs(p2.y-p1.y);
	int dy02 = abs(p3.y-p1.y);

	long double* x01 = priori::lerp<double>(p1.y, p1.x, p2.y, p2.x);
	long double* x02 = priori::lerp<double>(p1.y, p1.x, p3.y, p3.x);
	long double* x12 = priori::lerp<double>(p2.y, p2.x, p3.y, p3.x);

	for(int i = 0; i <= dy02; i++){
		int n0 = (int)x02[i];
		int n1 = (int)(i < dy01 ? x01[i] : x12[i-dy01]);
		int n = min(n0, n1);
		for(int j = 0; j < abs(n1-n0); j++)
			target.pixels[j+n][i+y0] = color;
	}
}

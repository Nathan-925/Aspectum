/*
 * Rasterizer.cpp
 *
 *  Created on: Dec 4, 2022
 *      Author: Nathan
 */
#include <cmath>

#include "BMP.h"
#include "Rasterizer.h"
#include "Interpolation.h"

using namespace std;

void raster::drawLine(uint32_t* target, uint16_t targetWidth, uint16_t targetHeight, Color color, int x0, int y0, int x1, int y1){
	int dx = x1-x0;
	int dy = y1-y0;
	if(abs(dx) > abs(dy)){
		long double* line = intp::interpolate<long double>(x0, y0, x1, y1);
		for(int i = 0; i <= abs(dx); i++)
			target[x0+(int)copysign(i, dx)+targetWidth*(int)line[i]] = color.c;
	}
	else{
		long double* line = intp::interpolate<long double>(y0, x0, y1, x1);
		for(int i = 0; i <= abs(dy); i++)
			target[(int)line[i]+targetWidth*(y0+(int)copysign(i, dy))] = color.c;
	}
}

void raster::drawTriangle(uint32_t* target, uint16_t targetWidth, uint16_t targetHeight, Color color, int x0, int y0, int x1, int y1, int x2, int y2){
	raster::drawLine(target, targetWidth, targetHeight, color, x0, y0, x1, y1);
	raster::drawLine(target, targetWidth, targetHeight, color, x0, y0, x2, y2);
	raster::drawLine(target, targetWidth, targetHeight, color, x1, y1, x2, y2);
}

void raster::fillTriangle(uint32_t* target, uint16_t targetWidth, uint16_t targetHeight, Color color, int x0, int y0, int x1, int y1, int x2, int y2){
	if(y1 < y0){
		int temp = x0;
		x0 = x1;
		x1 = temp;
		temp = y0;
		y0 = y1;
		y1 = temp;
	}
	if(y2 < y0){
		int temp = x0;
		x0 = x2;
		x2 = temp;
		temp = y0;
		y0 = y2;
		y2 = temp;
	}
	if(y2 < y1){
		int temp = x1;
		x1 = x2;
		x2 = temp;
		temp = y1;
		y1 = y2;
		y2 = temp;
	}

	int dy01 = abs(y1-y0);
	int dy02 = abs(y2-y0);

	long double* x01 = intp::interpolate<long double>(y0, x0, y1, x1);
	long double* x02 = intp::interpolate<long double>(y0, x0, y2, x2);
	long double* x12 = intp::interpolate<long double>(y1, x1, y2, x2);

	for(int i = 0; i <= dy02; i++){
		int n0 = (int)x02[i];
		int n1 = (int)(i < dy01 ? x01[i] : x12[i-dy01]);
		int n = min(n0, n1);
		for(int j = 0; j < abs(n1-n0); j++)
			target[j+n+targetWidth*(i+y0)] = color.c;
	}
}

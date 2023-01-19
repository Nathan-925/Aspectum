/*
 * Main.cpp
 *
 *  Created on: Dec 7, 2022
 *      Author: Nathan
 */
#include <iostream>
#include <cmath>

#include "Rasterizer.h"
#include "Projection.h"
#include "Shapes.h"
#include "priori/BMPLibrary.h"

using namespace std;
using namespace priori;

int main(){
	Image image(800, 800);

	raster::drawLine(image, 0, Point(100, 100), Point(200, 300));

	writebmp("test.bmp", image);
}

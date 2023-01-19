/*
 * Rasterizer.h
 *
 *  Created on: Dec 7, 2022
 *      Author: Nathan
 */

#ifndef RASTERIZER_H_
#define RASTERIZER_H_

#include <cstdint>

#include "priori/Graphical.h"
#include "priori/Math.h"

namespace raster{
	void drawLine(priori::Image target, priori::Color color, priori::Point p1, priori::Point p2);
	void drawTriangle(priori::Image target, priori::Color color, priori::Point p1, priori::Point p2, priori::Point p3);
	void fillTriangle(priori::Image target, priori::Color color, priori::Point p1, priori::Point p2, priori::Point p3);
}

#endif /* RASTERIZER_H_ */

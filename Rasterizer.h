/*
 * Rasterizer.h
 *
 *  Created on: Dec 7, 2022
 *      Author: Nathan
 */

#ifndef RASTERIZER_H_
#define RASTERIZER_H_

#include <cstdint>

#include "Color.h"

namespace raster{
	void drawLine(uint32_t* target, uint16_t targetWidth, uint16_t targetHeight, Color color, int x0, int y0, int x1, int y1);
	void drawTriangle(uint32_t* target, uint16_t targetWidth, uint16_t targetHeight, Color color, int x0, int y0, int x1, int y1, int x2, int y2);
	void fillTriangle(uint32_t* target, uint16_t targetWidth, uint16_t targetHeight, Color color, int x0, int y0, int x1, int y1, int x2, int y2);
}

#endif /* RASTERIZER_H_ */

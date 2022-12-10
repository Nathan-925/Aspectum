/*
 * Color.h
 *
 *  Created on: Dec 9, 2022
 *      Author: Nathan
 */

#ifndef COLOR_H_
#define COLOR_H_

#include <cstdint>

union Color{
	uint32_t c;
	struct{
		uint8_t b;
		uint8_t g;
		uint8_t r;
		uint8_t a;
	};

	Color() : c(0) {};
	Color(int c) : c(c) {};
	Color(int r, int g, int b) : b(b), g(g), r(r), a(0) {};

	Color operator*(const Color &other);
	Color operator/(const Color &other);
	Color operator+(const Color &other);
	Color operator-(const Color &other);

	Color operator*(const double &other);
	Color operator/(const double &other);

	Color operator+=(const Color &other);
	Color operator-=(const Color &other);

	Color operator*=(const double &other);
};



#endif /* COLOR_H_ */

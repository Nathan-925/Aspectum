/*
 * LightSource.cpp
 *
 *  Created on: Feb 4, 2023
 *      Author: Nathan
 */
#include <cmath>

#include "LightSource.h"

using namespace std;
using namespace priori;

namespace asp{

	LightSource::LightSource(Color c, double i) : color(c), intensity(i) {};

	AmbientLight::AmbientLight(Color c, double i) : LightSource(c, i) {};

	Color AmbientLight::getShade(Point3D point, Vector3D normal){
		return color*intensity;
	}

	DirectionalLight::DirectionalLight(Color c, double i, Vector3D v) : LightSource(c, i), direction(v) {};

	Color DirectionalLight::getShade(Point3D point, Vector3D normal){
		double d = (normal*direction)/(normal.magnitude()*direction.magnitude());
		Color c = color*d;
		return c;
	}

}

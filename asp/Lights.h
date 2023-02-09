/*
 * LightSource.h
 *
 *  Created on: Feb 4, 2023
 *      Author: Nathan
 */

#ifndef PRIORI_MATH3D_LIGHTSOURCE_H_
#define PRIORI_MATH3D_LIGHTSOURCE_H_

#include "priori/Graphical.h"
#include "priori/Math3D.h"

namespace asp{
	struct AmbientLight{
		priori::Color color;
		double intensity;
	};

	struct DirectionalLight{
		priori::Color color;
		double intensity;
		priori::Vector3D vector;
	};

	struct PointLight{
		priori::Color color;
		double intensity;
		priori::Point3D point;
	};
}

#endif /* PRIORI_MATH3D_LIGHTSOURCE_H_ */

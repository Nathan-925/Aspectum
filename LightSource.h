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
	class LightSource{
	public:
		priori::Color color;
		double intensity;

		LightSource(priori::Color c, double i);

		virtual priori::Color getShade(priori::Point3D point, priori::Vector3D normal)=0;
		virtual ~LightSource(){};
	};

	class AmbientLight : public LightSource{
	public:
		AmbientLight(priori::Color c, double i);

		priori::Color getShade(priori::Point3D point, priori::Vector3D normal);
	};

	class DirectionalLight : public LightSource{
	public:
		priori::Vector3D direction;

		DirectionalLight(priori::Color c, double i, priori::Vector3D v);

		priori::Color getShade(priori::Point3D point, priori::Vector3D normal);
	};
}

#endif /* PRIORI_MATH3D_LIGHTSOURCE_H_ */

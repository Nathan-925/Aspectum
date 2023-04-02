/*
 * LightSource.h
 *
 *  Created on: Feb 4, 2023
 *      Author: Nathan
 */

#ifndef PRIORI_MATH3D_LIGHTSOURCE_H_
#define PRIORI_MATH3D_LIGHTSOURCE_H_

#include "Model.h"

#include "priori/Graphical.h"
#include "priori/Math3D.h"

namespace asp{

	struct Light{
		priori::Color color;
		double intensity;

		Light(priori::Color c = 0xFFFFFF, double i = 1);
		Light(const Light &other);
		virtual ~Light();

		virtual Light* copy();
		virtual void diffuseShade(Fragment &fragment);
		virtual void specularShade(Fragment &fragment);
		virtual void transform(priori::TransformationMatrix transform);
	};

	struct DirectionalLight : public Light{
		priori::Vector3D vector;

		DirectionalLight(priori::Color c = 0xFFFFFF, double i = 1, priori::Vector3D v = priori::Vector3D{0, 1, 0});
		DirectionalLight(const DirectionalLight &other);
		~DirectionalLight();

		Light* copy();
		void diffuseShade(Fragment &fragment);
		void specularShade(Fragment &fragment);
		void transform(priori::TransformationMatrix transform);
	};

	struct PointLight : public Light{
		priori::Vector3D point;

		PointLight(priori::Color c = 0xFFFFFF, double i = 1, priori::Vector3D p = priori::Vector3D{0, 0, 0, true});
		PointLight(const PointLight &other);
		~PointLight();

		Light* copy();
		void diffuseShade(Fragment &fragment);
		void specularShade(Fragment &fragment);
		void transform(priori::TransformationMatrix transform);
	};

}

#endif /* PRIORI_MATH3D_LIGHTSOURCE_H_ */

/*
 * LightSource.cpp
 *
 *  Created on: Feb 4, 2023
 *      Author: Nathan
 */
#include <cmath>

#include "Lights.h"
#include "Camera.h"

using namespace std;
using namespace priori;

namespace asp{

	Light::Light(Color c, double i) : color(c), intensity(i) {};

	Light::Light(const Light &other) : color(other.color), intensity(other.intensity) {}

	Light::~Light() {};

	Light* Light::copy(){
		return new Light(*this);
	}

	void Light::diffuseShade(Fragment &fragment){
		fragment.color += fragment.material.ambient*Color(color*intensity);
	}

	void Light::specularShade(Fragment &fragment){}

	void Light::transform(priori::TransformationMatrix transform){}

	DirectionalLight::DirectionalLight(Color c, double i, Vector3D v) : Light(c, i), vector(v) {};

	DirectionalLight::DirectionalLight(const DirectionalLight &other) :
			Light(other.color, other.intensity),
			vector(other.vector)
	{}

	DirectionalLight::~DirectionalLight() {}

	Light* DirectionalLight::copy(){
		return new DirectionalLight(*this);
	}

	void DirectionalLight::diffuseShade(Fragment &fragment){
		double d = (vector*-1).normalize()*fragment.normal.normalize();
		if(d > 0)
			fragment.color += fragment.material.diffuse*(color*intensity*d);
	}

	void DirectionalLight::specularShade(Fragment &fragment){
		Vector3D toCamera = Vector3D{-(fragment.x-fragment.camera->viewPort.width/2.0)/(fragment.camera->focalLength*fragment.z),
									 -(fragment.camera->viewPort.height/2.0-fragment.y)/(fragment.camera->focalLength*fragment.z),
									 -1/fragment.z};
		Vector3D reflect = fragment.normal*2*(fragment.normal*vector) - vector;
		double d = -(reflect*toCamera)/(reflect.magnitude()*toCamera.magnitude());
		if(d > 0)
			fragment.color += fragment.material.specular*(color*intensity*pow(d, fragment.material.shine));
	}

	void DirectionalLight::transform(priori::TransformationMatrix transform){
		vector = transform*vector;
	}

	PointLight::PointLight(Color c, double i, Vector3D p) : Light(c, i), point(p) {};

	PointLight::PointLight(const PointLight &other) :
			Light(other.color, other.intensity),
			point(other.point)
	{}

	PointLight::~PointLight() {}

	Light* PointLight::copy(){
		return new PointLight(*this);
	}

	void PointLight::diffuseShade(Fragment &fragment){
		Vector3D vector = (point-Vector3D{(fragment.x-fragment.camera->viewPort.width/2)/(fragment.camera->focalLength*fragment.z),
										  (fragment.camera->viewPort.height/2-fragment.y)/(fragment.camera->focalLength*fragment.z),
										  1/fragment.z}).normalize();

		double d = vector*fragment.normal;
		if(d > 0)
			fragment.color += fragment.material.diffuse*(color*intensity*d);
	}

	void PointLight::specularShade(Fragment &fragment){
		Vector3D vector = (point-Vector3D{(fragment.x-fragment.camera->viewPort.width/2)/(fragment.camera->focalLength*fragment.z),
										  (fragment.camera->viewPort.height/2-fragment.y)/(fragment.camera->focalLength*fragment.z),
										  1/fragment.z}).normalize();

		Vector3D toCamera = Vector3D{-(fragment.x-fragment.camera->viewPort.width/2)/(fragment.camera->focalLength*fragment.z),
									 -(fragment.camera->viewPort.height/2-fragment.y)/(fragment.camera->focalLength*fragment.z),
									 -1/fragment.z};
		Vector3D reflect = fragment.normal*2*(fragment.normal*vector) - vector;
		double d = (reflect*toCamera)/(reflect.magnitude()*toCamera.magnitude());
		if(d > 0)
			fragment.color += fragment.material.specular*(color*intensity*pow(d, fragment.material.shine));
	}

	void PointLight::transform(priori::TransformationMatrix transform){
		point = transform*point;
	}

}

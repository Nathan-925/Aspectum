/*
 * Projection.cpp
 *
 *  Created on: Dec 9, 2022
 *      Author: Nathan
 */
#include <cmath>
#include <forward_list>
#include <iterator>
#include <algorithm>
#include <iostream>

#include "Projection.h"

#include "priori/Math.h"
#include "priori/Graphical.h"

using namespace std;
using namespace priori;

namespace asp{
Vertex Vertex::operator+(const Vertex &other) const{
	Vertex out(*this);
	out.position += other.position;
	out.texel += other.texel;
	out.normal += other.normal;
	return out;
}

Vertex Vertex::operator-(const Vertex &other) const{
	Vertex out(*this);
	out.position -= other.position;
	out.texel -= other.texel;
	out.normal -= other.normal;
	return out;
}

Vertex Vertex::operator+=(const Vertex &other){
	*this = *this+other;
	return *this;
}

Vertex Vertex::operator-=(const Vertex &other){
	*this = *this-other;
	return *this;
}

Vertex Vertex::operator*(const double &d) const{
	Vertex out(*this);
	out.position *= d;
	out.texel *= d;
	out.normal *= d;
	return out;
}

Vertex Vertex::operator/(const double &d) const{
	Vertex out(*this);
	out.position /= d;
	out.texel /= d;
	out.normal /= d;
	return out;
}

Vertex Vertex::operator*=(const double &d){
	*this = *this*d;
	return *this;
}

Vertex Vertex::operator/=(const double &d){
	*this = *this/d;
	return *this;
}

Vertex& Triangle::operator[](const int &n){
	return points[n];
}

Triangle operator*(const TransformationMatrix &transform, const Triangle &triangle){
	Triangle out = triangle;
	for(int i = 0; i < 3; i++){
		out[i].position = transform*triangle.points[i].position;
		out[i].normal = transform*triangle.points[i].normal;
	}
	return out;
}

Color Texture::getColor(double x, double y){
	return image.pixels[(int)round(x*(image.width-1))][(int)round(y*(image.height-1))];
}

Instance::Instance(Model* m) : model(m), transform() {};

Instance::Instance(Model* m, TransformationMatrix t) : model(m), transform(t) {};

Camera::Camera(int width, int height) :
		depthInverse(new double*[width]),
		viewPort(width, height),
		settings(),
		position(0, 0, 0),
		rx(0), ry(0), rz(0){
	for(int i = 0; i < width; i++)
		depthInverse[i] = new double[height];

	clear();

	setFOV(90);
};

Camera::~Camera(){
	for(int i = 0; i < viewPort.width; i++)
		delete[] depthInverse[i];
	delete[] depthInverse;
}

void Camera::project(Triangle &triangle){
	for(int i = 0; i < 3; i++){
		triangle[i].position = Point3D((int)(viewPort.width/2+((triangle[i].position.x*focalLength)/triangle[i].position.z)),
									   (int)(viewPort.height/2-((triangle[i].position.y*focalLength)/triangle[i].position.z)),
									   1/triangle[i].position.z);
		triangle[i].texel *= triangle[i].position.z;
	}
}

void Camera::setFOV(double fov){
	focalLength = (viewPort.width-1)/(2*tan(fov*(M_PI/360)));
}

void Camera::render(const Scene &scene){
	cout << "render" << endl;

	//instance transformation
	Model triangles;
	for(Instance* i: scene.objects)
		for(Triangle t: *i->model)
			triangles.push_front(i->transform*t);

	//camera space transformation
	TransformationMatrix cameraMatrix;
	for(Triangle &t: triangles)
		t = cameraMatrix*t;

	DirectionalLight* directionalLights = new DirectionalLight[scene.directionalLights.size()+scene.pointLights.size()];
	for(uint i = 0; i < scene.directionalLights.size(); i++){
		directionalLights[i] = *scene.directionalLights[i];
		directionalLights[i].vector = (cameraMatrix*directionalLights[i].vector*-1).normalize();
	}

	PointLight* pointLights = new PointLight[scene.pointLights.size()];
	for(uint i = 0; i < scene.pointLights.size(); i++){
		pointLights[i] = *scene.pointLights[i];
		pointLights[i].point = cameraMatrix*pointLights[i].point;
	}
	cout << "transformed" << endl;

	//culling
	double xAngle = atan((viewPort.width-1)/(2.0*focalLength));
	double yAngle = atan((viewPort.height-1)/(2.0*focalLength));

	double x = sin(xAngle), y = sin(yAngle);

	Plane cullingPlanes[] = {Plane(Vector3D(0, 0, 1), -1),
							 Plane(Vector3D(x, 0, cos(xAngle)), 0),
							 Plane(Vector3D(-x, 0, cos(xAngle)), 0),
							 Plane(Vector3D(0, y, cos(yAngle)), 0),
							 Plane(Vector3D(0, -y, cos(yAngle)), 0)};

	for(Plane plane: cullingPlanes){
		auto prev = triangles.before_begin();
		for(auto it = triangles.begin(); it != triangles.end(); prev = it++){
			Triangle t = *it;
			int culled[3];
			int numCulled = 0;
			for(int i = 0; i < 3; i++){
				Point3D pos = t[i].position;
				if(plane.distance(pos) < 0)
					culled[numCulled++] = i;
				else
					culled[2-i+numCulled] = i;
			}

			if(numCulled == 3){
				triangles.erase_after(prev);
				it = prev;
			}
			else if(numCulled == 2){
				it->points[culled[0]] = lerp<Vertex>(plane.intersectionPercent(t[culled[0]].position, t[culled[2]].position), t[culled[0]], t[culled[2]]);
				it->points[culled[1]] = lerp<Vertex>(plane.intersectionPercent(t[culled[1]].position, t[culled[2]].position), t[culled[1]], t[culled[2]]);
			}
			else if(numCulled == 1){
				Triangle clipping(t);
				clipping[0] = lerp<Vertex>(plane.intersectionPercent(t[culled[0]].position, t[culled[1]].position), t[culled[0]], t[culled[1]]);
				clipping[1] = lerp<Vertex>(plane.intersectionPercent(t[culled[0]].position, t[culled[2]].position), t[culled[0]], t[culled[2]]);
				clipping[2] = t[culled[1]];

				it->points[culled[0]] = clipping[1];

				triangles.insert_after(prev, clipping);
			}
		}
	}
	cout << "culled" << endl;

	for(Triangle &t: triangles)
		project(t);
	cout << "projected" << endl;

	if(settings->wireframe)
		for(Triangle &triangle: triangles)
			priori::drawTriangle(viewPort, triangle.material.diffuse,
								 Point(triangle[0].position.x, triangle[0].position.y),
								 Point(triangle[1].position.x, triangle[1].position.y),
								 Point(triangle[2].position.x, triangle[2].position.y));
	else
		for(Triangle &triangle: triangles){
			Vertex v0 = triangle[0], v1 = triangle[1], v2 = triangle[2];
			if(v0.position.y > v1.position.y)
				swap(v0, v1);
			if(v0.position.y > v2.position.y)
				swap(v0, v2);
			if(v1.position.y > v2.position.y)
				swap(v1, v2);

			int dy01 = v1.position.y-v0.position.y;
			int dy02 = v2.position.y-v0.position.y;
			int dy12 = v2.position.y-v1.position.y;

			forward_list<Vertex> l01 = lerp<Vertex>(0, v0, dy01, v1);
			forward_list<Vertex> l02 = lerp<Vertex>(0, v0, dy02, v2);
			forward_list<Vertex> l12 = lerp<Vertex>(0, v1, dy12, v2);

			auto it01 = l01.begin();
			auto it02 = l02.begin();
			auto it12 = l12.begin();

			for(int i = 0; i <= dy02; i++){
				Vertex v1 = *it02++;
				Vertex v2 = i < dy01 ? *it01++ : *it12++;
				if(v1.position.x > v2.position.x)
					swap(v1, v2);

				forward_list<Vertex> line = lerp<Vertex>(v1.position.x, v1, v2.position.x, v2);
				int y = v0.position.y+i;
				for(Vertex v: line){
					int x = v.position.x;

					if(x < 0 || x >= viewPort.width || y < 0 || y >= viewPort.height)
						cout << "out " << x << " " << y << endl;

					if(v.position.z > depthInverse[x][y]){
						depthInverse[x][y] = v.position.z;

						Color ambient = triangle.material.ambient,
							  diffuse = triangle.material.diffuse,
							  specular = triangle.material.specular;
						if(settings->textures){
							v.texel /= v.position.z;
							if(triangle.material.ambientTexture != nullptr)
								ambient *= triangle.material.ambientTexture->getColor(v.texel.x, v.texel.y);
							if(triangle.material.diffuseTexture != nullptr)
								diffuse *= triangle.material.diffuseTexture->getColor(v.texel.x, v.texel.y);
							if(triangle.material.specularTexture != nullptr)
								specular *= triangle.material.specularTexture->getColor(v.texel.x, v.texel.y);
						}

						if(settings->shading && triangle.material.illuminationModel != 0){
							viewPort[x][y] = ambient*(scene.ambientLight.color*scene.ambientLight.intensity);

							for(uint i = 0; i < scene.pointLights.size(); i++){
								directionalLights[scene.directionalLights.size()+i].vector = (pointLights[i].point-Point3D((x-viewPort.width/2)/(focalLength*v.position.z),
																					 	 	 	 	  	  	  	   (viewPort.height/2-y)/(focalLength*v.position.z),
																												   1/v.position.z)).normalize();
								directionalLights[scene.directionalLights.size()+i].intensity = pointLights[i].intensity;
								directionalLights[scene.directionalLights.size()+i].color = pointLights[i].color;
							}

							Color diffuseSum = 0;

							for(uint i = 0; i < scene.directionalLights.size()+scene.pointLights.size(); i++){
								double d = directionalLights[i].vector*v.normal.normalize();
								if(d > 0)
									diffuseSum += (directionalLights[i].color*directionalLights[i].intensity*d);
							}

							viewPort[x][y] += diffuse*diffuseSum;

							if(settings->specular && triangle.material.illuminationModel >= 2){
								Color specularSum = 0;
								for(uint i = 0; i < scene.directionalLights.size()+scene.pointLights.size(); i++){
									Vector3D toCamera = position - Point3D((x-viewPort.width/2)/(focalLength*v.position.z),
																		   (viewPort.height/2-y)/(focalLength*v.position.z),
																		   1/v.position.z);
									Vector3D reflect = v.normal*2*(v.normal*directionalLights[i].vector) - directionalLights[i].vector;
									double d = (reflect*toCamera)/(reflect.magnitude()*toCamera.magnitude());
									cout << d << endl;
									if(d > 0){
										cout << "here" << endl;
										specularSum += directionalLights[i].color*pow(d, triangle.material.shine);
									}
								}
								viewPort[x][y] += specular*specularSum;
							}
						}
						else
							viewPort[x][y] = diffuse;
					}
				}
			}
		}
	delete[] directionalLights;
	delete[] pointLights;
}

Color Camera::shade(Point3D point, Vector3D normal, Material* material){
	Color c = 0;

	return c;
}

void Camera::clear(){
	for(int i = 0; i < viewPort.width; i++){
		for(int j = 0; j < viewPort.height; j++){
			viewPort.pixels[i][j] = 0xFFFFFF;
			depthInverse[i][j] = 0;
		}
	}
}
}

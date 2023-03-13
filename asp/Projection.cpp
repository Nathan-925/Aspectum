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
	return *vertices[n];
}

const Vertex& Triangle::operator[](const int &n) const{
	return *vertices[n];
}

Vertex operator*(const TransformationMatrix &transform, Vertex vertex){
	vertex.position = transform*vertex.position;
	vertex.normal = transform*vertex.normal;
	return vertex;
}

Vertex operator*=(Vertex &vertex, const priori::TransformationMatrix &transform){
	vertex.position = transform*vertex.position;
	vertex.normal = transform*vertex.normal;
	return vertex;
}

Triangle operator*(const TransformationMatrix &transform, const Triangle &triangle){
	Triangle out = triangle;
	for(int i = 0; i < 3; i++){
		out[i].position = transform*triangle[i].position;
		out[i].normal = transform*triangle[i].normal;
	}
	return out;
}

Model::Model() : vertices(), triangles(){};

Model::Model(const Model &other) : vertices(oth)

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
}

Camera::~Camera(){
	for(int i = 0; i < viewPort.width; i++)
		delete[] depthInverse[i];
	delete[] depthInverse;
}

void Camera::project(Triangle &triangle){
	for(int i = 0; i < 3; i++){
		cout << triangle[i].position.x << " " << triangle[i].position.y << " " << triangle[i].position.z << endl;
		triangle[i].position = Point3D((int)(viewPort.width/2+((triangle[i].position.x*focalLength)/triangle[i].position.z)),
									   (int)(viewPort.height/2-((triangle[i].position.y*focalLength)/triangle[i].position.z)),
									   1/triangle[i].position.z);
		cout << triangle[i].position.x << " " << triangle[i].position.y << " " << triangle[i].position.z << endl;
		triangle[i].texel *= triangle[i].position.z;
		triangle[i].normal = triangle[i].normal.normalize();
	}
}

void Camera::setFOV(double fov){
	focalLength = (viewPort.width-1)/(2*tan(fov*(M_PI/360)));
}

void Camera::render(const Scene &scene){
	cout << "render" << endl;

	TransformationMatrix cameraMatrix = translate(-position.x, -position.y, -position.z)*
										rotateZ(rz)*
										rotateY(ry)*
										rotateX(rx);

	for(Instance* instance: scene.objects){
		cout << instance->model->vertices.size() << endl;

		cout << instance->model->vertices[0].position.x << " " << instance->model->vertices[0].position.y << " " << instance->model->vertices[0].position.z << endl;
		//transformation
		TransformationMatrix transform = instance->transform*cameraMatrix;

		Model model;
		for(Vertex v: instance->model->vertices)
			model.vertices.push_back(transform*v);
		for(Triangle t: instance->model->triangles)
			model.triangles.push_front(t);
		cout << distance(model.triangles.begin(), model.triangles.end()) << endl;

		cout << model.vertices[0].position.x << " " << model.vertices[0].position.y << " " << model.vertices[0].position.z << endl;

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
		cout << distance(model.triangles.begin(), model.triangles.end()) << endl;
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

		cout << distance(model.triangles.begin(), model.triangles.end()) << endl;
		for(Plane plane: cullingPlanes){
			auto prev = model.triangles.before_begin();
			for(auto it = model.triangles.begin(); it != model.triangles.end(); prev = it++){
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
					model.triangles.erase_after(prev);
					it = prev;
				}
				else if(numCulled == 2){
					model.vertices.push_back(lerp<Vertex>(plane.intersectionPercent(t[culled[0]].position, t[culled[2]].position), t[culled[0]], t[culled[2]]));
					(*it)[culled[0]] = model.vertices.back();
					model.vertices.push_back(lerp<Vertex>(plane.intersectionPercent(t[culled[1]].position, t[culled[2]].position), t[culled[1]], t[culled[2]]));
					(*it)[culled[1]] = model.vertices.back();
				}
				else if(numCulled == 1){
					Triangle clipping(t);
					model.vertices.push_back(lerp<Vertex>(plane.intersectionPercent(t[culled[0]].position, t[culled[1]].position), t[culled[0]], t[culled[1]]));
					clipping[0] = model.vertices.back();
					model.vertices.push_back(lerp<Vertex>(plane.intersectionPercent(t[culled[0]].position, t[culled[2]].position), t[culled[0]], t[culled[2]]));
					clipping[2] = model.vertices.back();
					clipping[2] = t[culled[1]];

					(*it)[culled[0]] = clipping[1];

					model.triangles.insert_after(prev, clipping);
				}
			}
		}
		cout << "culled" << endl;

		cout << distance(model.triangles.begin(), model.triangles.end()) << endl;
		cout << distance(instance->model->triangles.begin(), instance->model->triangles.end()) << endl;
		for(Triangle &t: model.triangles)
			project(t);
		cout << "projected" << endl;

		if(settings->wireframe)
			for(Triangle &triangle: model.triangles)
				priori::drawTriangle(viewPort, triangle.material.diffuse,
									 Point(triangle[0].position.x, triangle[0].position.y),
									 Point(triangle[1].position.x, triangle[1].position.y),
									 Point(triangle[2].position.x, triangle[2].position.y));
		else
			for(Triangle &triangle: model.triangles){
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
								viewPort[x][y] = ambient*Color(scene.ambientLight.color*scene.ambientLight.intensity);

								for(uint i = 0; i < scene.pointLights.size(); i++){
									directionalLights[scene.directionalLights.size()+i].vector = (pointLights[i].point-Point3D((x-viewPort.width/2)/(focalLength*v.position.z),
																													   (viewPort.height/2-y)/(focalLength*v.position.z),
																													   1/v.position.z)).normalize();
									directionalLights[scene.directionalLights.size()+i].intensity = pointLights[i].intensity;
									directionalLights[scene.directionalLights.size()+i].color = pointLights[i].color;
								}

								Color diffuseSum = 0;

								for(uint i = 0; i < scene.directionalLights.size()+scene.pointLights.size(); i++){
									double d = directionalLights[i].vector*v.normal;
									if(d > 0)
										diffuseSum += (directionalLights[i].color*directionalLights[i].intensity*d);
								}

								viewPort[x][y] += diffuse*diffuseSum;

								if(settings->specular && triangle.material.illuminationModel >= 2){
									Color specularSum = 0;
									for(uint i = 0; i < scene.directionalLights.size()+scene.pointLights.size(); i++){
										Vector3D toCamera = Point3D(-(x-viewPort.width/2)/(focalLength*v.position.z),
																	-(viewPort.height/2-y)/(focalLength*v.position.z),
																	-1/v.position.z);
										Vector3D reflect = v.normal*2*(v.normal*directionalLights[i].vector) - directionalLights[i].vector;
										double d = (reflect*toCamera)/(reflect.magnitude()*toCamera.magnitude());
										if(d > 0){
											specularSum += directionalLights[i].color*directionalLights[i].intensity*pow(d, triangle.material.shine);
											cout << hex << uint32_t(directionalLights[i].color*directionalLights[i].intensity*pow(d, triangle.material.shine)) << dec << endl;
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

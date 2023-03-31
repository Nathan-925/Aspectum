/*
 * Projection.cpp
 *
 *  Created on: Dec 9, 2022
 *      Author: Nathan
 */
#include <cmath>
#include <iterator>
#include <algorithm>
#include <forward_list>
#include <iostream>

#include "Projection.h"

#include "priori/Math.h"
#include "priori/Graphical.h"

using namespace std;
using namespace priori;

namespace asp{
Vertex Vertex::operator+(Vertex other) const{
	other.position += position;
	other.texel += texel;
	return other;
}

Vertex Vertex::operator-(Vertex other) const{
	other.position = position-other.position;
	other.texel = texel-other.texel;
	return other;
}

Vertex Vertex::operator+=(const Vertex &other){
	position += other.position;
	texel += other.texel;
	return *this;
}

Vertex Vertex::operator-=(const Vertex &other){
	position -= other.position;
	texel -= other.texel;
	return *this;
}

Vertex Vertex::operator*(const double &d) const{
	Vertex out(*this);
	out.position *= d;
	out.texel *= d;
	return out;
}

Vertex Vertex::operator/(const double &d) const{
	Vertex out(*this);
	out.position /= d;
	out.texel /= d;
	return out;
}

Vertex Vertex::operator*=(const double &d){
	position *= d;
	texel *= d;
	return *this;
}

Vertex Vertex::operator/=(const double &d){
	position /= d;
	texel /= d;
	return *this;
}

Vertex operator*(const TransformationMatrix &transform, Vertex vertex){
	vertex.position = transform*vertex.position;
	return vertex;
}

Vertex operator*=(Vertex &vertex, const priori::TransformationMatrix &transform){
	vertex.position = transform*vertex.position;
	return vertex;
}

Fragment Fragment::operator+(Fragment other) const{
	other.position += position;
	other.texel += texel;
	other.normal += normal;
	return other;
}

Fragment Fragment::operator-(Fragment other) const{
	other.position = position-other.position;
	other.texel = texel-other.texel;
	other.normal = normal-other.normal;
	return other;
}

Fragment Fragment::operator+=(const Fragment &other){
	position += other.position;
	texel += other.texel;
	normal += other.normal;
	return *this;
}

Fragment Fragment::operator-=(const Fragment &other){
	position -= other.position;
	texel -= other.texel;
	normal -= other.normal;
	return *this;
}

Fragment Fragment::operator*(const double &d) const{
	Fragment out(*this);
	out.position *= d;
	out.texel *= d;
	out.normal *= d;
	return out;
}

Fragment Fragment::operator/(const double &d) const{
	Fragment out(*this);
	out.position /= d;
	out.texel /= d;
	out.normal /= d;
	return out;
}

Fragment Fragment::operator*=(const double &d){
	position *= d;
	texel *= d;
	normal *= d;
	return *this;
}

Fragment Fragment::operator/=(const double &d){
	position /= d;
	texel /= d;
	normal /= d;
	return *this;
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
}

Camera::~Camera(){
	for(int i = 0; i < viewPort.width; i++)
		delete[] depthInverse[i];
	delete[] depthInverse;
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
		vector<Vertex> vertices(instance->model->vertices);
		forward_list<Triangle> triangles;

		//transformation
		TransformationMatrix transform = instance->transform*cameraMatrix;
		for(Vertex &v: vertices)
			v = transform*v;
		for(Triangle t: instance->model->triangles){
			triangles.push_front(t);
			for(Vector3D &v: triangles.front().normals)
				v = transform*v;
		}
		cout << vertices.size() << " " << distance(triangles.begin(), triangles.end()) << endl;

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
					Point3D pos = vertices[t.vertices[i]].position;
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
					(*it).vertices[culled[0]] = vertices.size();
					vertices.push_back(
							lerp<Vertex>(plane.intersectionPercent(vertices[t.vertices[culled[0]]].position,
													  	  	  	   vertices[t.vertices[culled[2]]].position),
							vertices[t.vertices[culled[0]]],
							vertices[t.vertices[culled[2]]]));

					(*it).vertices[culled[1]] = vertices.size();
					vertices.push_back(
							lerp<Vertex>(plane.intersectionPercent(vertices[t.vertices[culled[1]]].position,
													  	  	  	   vertices[t.vertices[culled[2]]].position),
							vertices[t.vertices[culled[1]]],
							vertices[t.vertices[culled[2]]]));
				}
				else if(numCulled == 1){
					Triangle clipping(t);

					clipping.vertices[0] = vertices.size();
					vertices.push_back(
							lerp<Vertex>(plane.intersectionPercent(vertices[t.vertices[culled[0]]].position,
													  	  	  	   vertices[t.vertices[culled[1]]].position),
							vertices[t.vertices[culled[0]]],
							vertices[t.vertices[culled[1]]]));

					clipping.vertices[1] = vertices.size();
					vertices.push_back(
							lerp<Vertex>(plane.intersectionPercent(vertices[t.vertices[culled[0]]].position,
													  	  	  	   vertices[t.vertices[culled[2]]].position),
							vertices[t.vertices[culled[0]]],
							vertices[t.vertices[culled[2]]]));

					clipping.vertices[2] = t.vertices[culled[1]];
					(*it).vertices[culled[0]] = clipping.vertices[1];

					cout << vertices[clipping.vertices[0]].position.x << " " << vertices[clipping.vertices[0]].position.y << " " << vertices[clipping.vertices[0]].position.z << endl;

					triangles.push_front(clipping);
				}
			}
		}
		cout << "culled" << endl;

		for(Vertex &v: vertices){
			cout << v.position.x << " " << v.position.y << " " << v.position.z << " -> ";
			v.position = Point3D((int)(viewPort.width/2+((v.position.x*focalLength)/v.position.z)),
								 (int)(viewPort.height/2-((v.position.y*focalLength)/v.position.z)),
								 1/v.position.z);
			v.texel *= v.position.z;
			cout << v.position.x << " " << v.position.y << " " << v.position.z << endl;
		}
		cout << "projected" << endl;

		if(settings->wireframe)
			for(Triangle &triangle: triangles)
				priori::drawTriangle(viewPort, triangle.material.diffuse,
									 Point(vertices[triangle.vertices[0]].position.x, vertices[triangle.vertices[0]].position.y),
									 Point(vertices[triangle.vertices[1]].position.x, vertices[triangle.vertices[1]].position.y),
									 Point(vertices[triangle.vertices[2]].position.x, vertices[triangle.vertices[2]].position.y));
		else
			for(Triangle &triangle: triangles){
				Fragment f0 = Fragment{vertices[triangle.vertices[0]].position, vertices[triangle.vertices[0]].texel, triangle.normals[0].normalize(), 0},
						 f1 = Fragment{vertices[triangle.vertices[1]].position, vertices[triangle.vertices[1]].texel, triangle.normals[1].normalize(), 0},
						 f2 = Fragment{vertices[triangle.vertices[2]].position, vertices[triangle.vertices[2]].texel, triangle.normals[2].normalize(), 0};

				if(f0.position.y > f1.position.y)
					swap(f0, f1);
				if(f0.position.y > f2.position.y)
					swap(f0, f2);
				if(f1.position.y > f2.position.y)
					swap(f1, f2);

				int dy01 = f1.position.y-f0.position.y;
				int dy02 = f2.position.y-f0.position.y;
				int dy12 = f2.position.y-f1.position.y;

				forward_list<Fragment> l01 = lerp<Fragment>(0, f0, dy01, f1);
				forward_list<Fragment> l02 = lerp<Fragment>(0, f0, dy02, f2);
				forward_list<Fragment> l12 = lerp<Fragment>(0, f1, dy12, f2);

				auto it01 = l01.begin();
				auto it02 = l02.begin();
				auto it12 = l12.begin();

				for(int i = 0; i <= dy02; i++){
					Fragment f1 = *it02++;
					Fragment f2 = i < dy01 ? *it01++ : *it12++;
					if(f1.position.x > f2.position.x)
						swap(f1, f2);

					forward_list<Fragment> line = lerp<Fragment>(f1.position.x, f1, f2.position.x, f2);
					int y = f0.position.y+i;
					for(Fragment f: line){
						int x = f.position.x;

						if(x < 0 || x >= viewPort.width || y < 0 || y >= viewPort.height){
							cout << x << " " << y << endl;
							throw "pixel drawn out of bounds";
						}

						if(f.normal.z < 0 && f.position.z > depthInverse[x][y]){
							depthInverse[x][y] = f.position.z;

							Color ambient = triangle.material.ambient,
								  diffuse = triangle.material.diffuse,
								  specular = triangle.material.specular;
							if(settings->textures){
								f.texel /= f.position.z;
								if(triangle.material.ambientTexture != nullptr)
									ambient *= triangle.material.ambientTexture->getColor(f.texel.x, f.texel.y);
								if(triangle.material.diffuseTexture != nullptr)
									diffuse *= triangle.material.diffuseTexture->getColor(f.texel.x, f.texel.y);
								if(triangle.material.specularTexture != nullptr)
									specular *= triangle.material.specularTexture->getColor(f.texel.x, f.texel.y);
							}

							if(settings->shading && triangle.material.illuminationModel != 0){
								f.color = ambient*Color(scene.ambientLight.color*scene.ambientLight.intensity);

								for(uint i = 0; i < scene.pointLights.size(); i++){
									directionalLights[scene.directionalLights.size()+i].vector = (pointLights[i].point-Point3D((x-viewPort.width/2)/(focalLength*f.position.z),
																													   (viewPort.height/2-y)/(focalLength*f.position.z),
																													   1/f.position.z)).normalize();
									directionalLights[scene.directionalLights.size()+i].intensity = pointLights[i].intensity;
									directionalLights[scene.directionalLights.size()+i].color = pointLights[i].color;
								}

								Color diffuseSum = 0;

								for(uint i = 0; i < scene.directionalLights.size()+scene.pointLights.size(); i++){
									double d = directionalLights[i].vector*f.normal;
									if(d > 0)
										diffuseSum += (directionalLights[i].color*directionalLights[i].intensity*d);
								}

								f.color += diffuse*diffuseSum;

								if(settings->specular && triangle.material.illuminationModel >= 2){
									Color specularSum = 0;
									for(uint i = 0; i < scene.directionalLights.size()+scene.pointLights.size(); i++){
										Vector3D toCamera = Point3D(-(x-viewPort.width/2)/(focalLength*f.position.z),
																	-(viewPort.height/2-y)/(focalLength*f.position.z),
																	-1/f.position.z);
										Vector3D reflect = f.normal*2*(f.normal*directionalLights[i].vector) - directionalLights[i].vector;
										double d = (reflect*toCamera)/(reflect.magnitude()*toCamera.magnitude());
										if(d > 0)
											specularSum += directionalLights[i].color*directionalLights[i].intensity*pow(d, triangle.material.shine);
									}
									f.color += specular*specularSum;
								}
							}
							else
								f.color = diffuse;

							for(void (*shader)(Fragment&): fragmentShaders)
								shader(f);

							viewPort[x][y] = f.color;
						}
					}
				}
		}
		delete[] directionalLights;
		delete[] pointLights;
	}
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

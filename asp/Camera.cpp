/*
 * Camera.cpp
 *
 *  Created on: Mar 31, 2023
 *      Author: Nathan
 */
#include <iostream>

#include "Camera.h"

#include "priori/Math.h"
#include "priori/Math3D.h"

using namespace std;
using namespace priori;

namespace asp{

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

			Light** lights = new Light*[scene.lights.size()];
			for(uint i = 0; i < scene.lights.size(); i++){
				lights[i] = scene.lights[i]->copy();
				lights[i]->transform(cameraMatrix);
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

						triangles.push_front(clipping);
					}
				}
			}
			cout << "culled" << endl;

			for(Vertex &v: vertices){
				v.position = Point3D((int)(viewPort.width/2+((v.position.x*focalLength)/v.position.z)),
									 (int)(viewPort.height/2-((v.position.y*focalLength)/v.position.z)),
									 1/v.position.z);
				v.texel *= v.position.z;
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
					Fragment f0 = Fragment{this, vertices[triangle.vertices[0]].position, vertices[triangle.vertices[0]].texel, triangle.normals[0].normalize(), 0, triangle.material},
							 f1 = Fragment{this, vertices[triangle.vertices[1]].position, vertices[triangle.vertices[1]].texel, triangle.normals[1].normalize(), 0, triangle.material},
							 f2 = Fragment{this, vertices[triangle.vertices[2]].position, vertices[triangle.vertices[2]].texel, triangle.normals[2].normalize(), 0, triangle.material};

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

								if(settings->textures){
									f.texel /= f.position.z;
									if(triangle.material.ambientTexture != nullptr)
										f.material.ambient *= triangle.material.ambientTexture->getColor(f.texel.x, f.texel.y);
									if(triangle.material.diffuseTexture != nullptr)
										f.material.diffuse *= triangle.material.diffuseTexture->getColor(f.texel.x, f.texel.y);
									if(triangle.material.specularTexture != nullptr)
										f.material.specular *= triangle.material.specularTexture->getColor(f.texel.x, f.texel.y);
								}

								if(settings->shading && triangle.material.illuminationModel != 0){
									for(uint i = 0; i < scene.lights.size(); i++)
										lights[i]->diffuseShade(f);

									if(settings->specular && triangle.material.illuminationModel >= 2)
										for(uint i = 0; i < scene.lights.size(); i++)
											lights[i]->specularShade(f);
								}
								else
									f.color = f.material.diffuse;

								for(void (*shader)(Fragment&): fragmentShaders)
									shader(f);

								viewPort[x][y] = f.color;
							}
						}
					}
			}
			for(uint i = 0; i < scene.lights.size(); i++)
				delete lights[i];
			delete[] lights;
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

/*
 * Camera.cpp
 *
 *  Created on: Mar 31, 2023
 *      Author: Nathan
 */
#include <numbers>
#include <iostream>

#include "Camera.h"
#include "Shaders.h"

#include "priori/Math.h"
#include "priori/Math3D.h"

using namespace std;
using namespace priori;

namespace asp{

	Camera::Camera(int width, int height) :
			depthInverse(new double*[width]),
			viewPort(width, height),
			settings(),
			position{0, 0, 0, true},
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

	Fragment Camera::project(const Vertex &vertex, Vector3D normal, const Material &material){
		Fragment f{this,
						Vector3D{(double)((int)(viewPort.width/2+((vertex.position.x*focalLength)/vertex.position.z))),
								 (double)((int)(viewPort.height/2-((vertex.position.y*focalLength)/vertex.position.z))),
								 1/vertex.position.z, true},
						vertex.texel/vertex.position.z,
						normal.normalize(),
						0,
						material};
		cout << vertex.position.x << " " << vertex.position.y << " " << vertex.position.z << " -> ";
		cout << f.position.x << " " << f.position.y << " " << f.position.z << endl;
		return Fragment{this,
						Vector3D{(double)((int)(viewPort.width/2+((vertex.position.x*focalLength)/vertex.position.z))),
								 (double)((int)(viewPort.height/2-((vertex.position.y*focalLength)/vertex.position.z))),
								 1/vertex.position.z, true},
						vertex.texel/vertex.position.z,
						normal.normalize(),
						0,
						material};
	}

	void Camera::setFOV(double fov){
		focalLength = (viewPort.width-1)/(2*tan(fov*(numbers::pi/360)));
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
			for(unsigned int i = 0; i < scene.lights.size(); i++){
				lights[i] = scene.lights[i]->copy();
				lights[i]->transform(cameraMatrix);
			}
			cout << "transformed" << endl;

			//culling
			double xAngle = atan((viewPort.width-1)/(2.0*focalLength));
			double yAngle = atan((viewPort.height-1)/(2.0*focalLength));

			double x = sin(xAngle), y = sin(yAngle);

			CullingPlane cullingPlanes[] = {CullingPlane{Vector3D{0, 0, 1, false}, -1},
									 	    CullingPlane{Vector3D{x, 0, cos(xAngle), false}, 0},
											CullingPlane{Vector3D{-x, 0, cos(xAngle), false}, 0},
											CullingPlane{Vector3D{0, y, cos(yAngle), false}, 0},
											CullingPlane{Vector3D{0, -y, cos(yAngle), false}, 0}};

			for(Vertex v: vertices)
				cout << v.position.x << " " << v.position.y << " " << v.position.z << endl;

			for(CullingPlane plane: cullingPlanes)
				plane.cull(vertices, triangles);
			cout << "culled" << endl;

			for(Vertex v: vertices)
				cout << v.position.x << " " << v.position.y << " " << v.position.z << endl;
			for(Triangle t: triangles)
				cout << t.vertices[0] << " " << t.vertices[1] << " " << t.vertices[2] << endl;

			for(Triangle &triangle: triangles){
				Fragment f0 = project(vertices[triangle.vertices[0]], triangle.normals[0], triangle.material),
						 f1 = project(vertices[triangle.vertices[1]], triangle.normals[1], triangle.material),
						 f2 = project(vertices[triangle.vertices[2]], triangle.normals[2], triangle.material);

				if(f0.position.y > f1.position.y)
					swap(f0, f1);
				if(f0.position.y > f2.position.y)
					swap(f0, f2);
				if(f1.position.y > f2.position.y)
					swap(f1, f2);

				int dy01 = f1.position.y-f0.position.y;
				int dy02 = f2.position.y-f0.position.y;
				int dy12 = f2.position.y-f1.position.y;

				forward_list<forward_list<Fragment>> lines;
				if(settings->wireframe){
					f0.normal = Vector3D{0, 0, -1};
					f1.normal = Vector3D{0, 0, -1};
					f2.normal = Vector3D{0, 0, -1};
					lines.push_front(lerp<Fragment>(0, f0, dy01, f1));
					lines.push_front(lerp<Fragment>(0, f0, dy02, f2));
					lines.push_front(lerp<Fragment>(0, f1, dy12, f2));
					cout << distance(lines.begin(), lines.end()) << endl;
				}
				else{
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
						lines.push_front(lerp<Fragment>(f1.position.x, f1, f2.position.x, f2));
						cout << i << endl;
					}
				}

				for(forward_list<Fragment> line: lines){
					for(Fragment f: line){
						int x = f.position.x, y = f.position.y;
						cout << x << " " << y << " " << f.position.z << " " << depthInverse[x][y] << endl;

						if(x < 0 || x >= viewPort.width || y < 0 || y >= viewPort.height){
							cout << "out " << x << " " << y << endl;
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

							if(!settings->wireframe && settings->shading && triangle.material.illuminationModel != 0){
								for(unsigned int i = 0; i < scene.lights.size(); i++)
									lights[i]->diffuseShade(f);

								if(settings->specular && triangle.material.illuminationModel >= 2)
									for(unsigned int i = 0; i < scene.lights.size(); i++)
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
			for(unsigned int i = 0; i < scene.lights.size(); i++)
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

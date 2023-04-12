/*
 * Camera.cpp
 *
 *  Created on: Mar 31, 2023
 *      Author: Nathan
 */
#include <cmath>
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

	Fragment Camera::project(const Vertex &vertex, Vector3D normal, priori::Vector texel, const Material &material){
		return Fragment{this,
						Vector3D{(double)((int)(viewPort.width/2+((vertex.position.x*focalLength)/vertex.position.z))),
								 (double)((int)(viewPort.height/2+((vertex.position.y*focalLength)/vertex.position.z))),
								 1/vertex.position.z, true},
						texel/vertex.position.z,
						normal.normalize(),
						0,
						material};
	}

	void Camera::setFOV(double fov){
		focalLength = (viewPort.width-1)/(2*tan(fov*(M_PI/360)));
	}

	void Camera::render(const Scene &scene){
		cout << "render" << endl;

		TransformationMatrix cameraMatrix = translate(-position.x, -position.y, -position.z)*
											rotateY(ry)*
											rotateX(rx)*
											rotateZ(rz);

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

			for(VertexShader shader: vertexShaders)
				for(Vertex &v: vertices)
					shader(v);

			//culling
			if(settings->backFaceCulling)
				cullBackFaces(vertices, triangles);

			double xAngle = atan((viewPort.width-1)/(2.0*focalLength));
			double yAngle = atan((viewPort.height-1)/(2.0*focalLength));

			double xCos = cos(xAngle), yCos = cos(yAngle);
			double xSin = sin(xAngle), ySin = sin(yAngle);

			CullingPlane cullingPlanes[] = {CullingPlane{Vector3D{0, 0, 1, false}, -1},
									 	    CullingPlane{Vector3D{xCos, 0, xSin, false}, 0},
											CullingPlane{Vector3D{-xCos, 0, xSin, false}, 0},
											CullingPlane{Vector3D{0, yCos, ySin, false}, 0},
											CullingPlane{Vector3D{0, -yCos, ySin, false}, 0}};

			for(CullingPlane plane: cullingPlanes)
				plane.cull(vertices, triangles);
			cout << "culled" << endl;

			for(Triangle &triangle: triangles){
				Fragment f0 = project(vertices[triangle.vertices[0]], triangle.normals[0], triangle.texels[0], triangle.material),
						 f1 = project(vertices[triangle.vertices[1]], triangle.normals[1], triangle.texels[1], triangle.material),
						 f2 = project(vertices[triangle.vertices[2]], triangle.normals[2], triangle.texels[2], triangle.material);

				if(f0.position.y > f1.position.y)
					swap(f0, f1);
				if(f0.position.y > f2.position.y)
					swap(f0, f2);
				if(f1.position.y > f2.position.y)
					swap(f1, f2);

				int dx01 = abs(f1.position.x-f0.position.x);
				int dx02 = abs(f2.position.x-f0.position.x);
				int dx12 = abs(f2.position.x-f1.position.x);

				int dy01 = f1.position.y-f0.position.y;
				int dy02 = f2.position.y-f0.position.y;
				int dy12 = f2.position.y-f1.position.y;

				Fragment x1 = f0.texel.x == f1.texel.x ? f2 : f1;
				Fragment y1 = f0.texel.y == f1.texel.y ? f2 : f1;

				double dx = abs(x1.position.x-f0.position.x);
				double dy = abs(y1.position.y-f0.position.y);
				Vector dtx = Vector{dx, dx}/max(1.0, abs(x1.texel.x/x1.position.z-f0.texel.x/f0.position.z));
				Vector dty = Vector{dy, dy}/max(1.0, abs(y1.texel.y/y1.position.z-f0.texel.y/f0.position.z));

				vector<vector<Fragment>> lines;
				if(settings->wireframe){
					f0.normal = Vector3D{0, 0, -1};
					f1.normal = Vector3D{0, 0, -1};
					f2.normal = Vector3D{0, 0, -1};
					lines.push_back(dy01 > dx01 ? lerp<Fragment>(0, f0, dy01, f1) : lerp<Fragment>(0, f0, dx01, f1));
					lines.push_back(dy02 > dx02 ? lerp<Fragment>(0, f0, dy02, f2) : lerp<Fragment>(0, f0, dx02, f2));
					lines.push_back(dy12 > dx12 ? lerp<Fragment>(0, f1, dy12, f2) : lerp<Fragment>(0, f1, dx12, f2));
				}
				else{
					vector<Fragment> l01 = lerp<Fragment>(0, f0, dy01, f1);
					vector<Fragment> l02 = lerp<Fragment>(0, f0, dy02, f2);
					vector<Fragment> l12 = lerp<Fragment>(0, f1, dy12, f2);

					auto it01 = l01.begin();
					auto it02 = l02.begin();
					auto it12 = l12.begin();

					for(int i = 0; i <= dy02; i++){
						Fragment f1 = *it02++;
						Fragment f2 = i < dy01 ? *it01++ : *it12++;
						if(f1.position.x > f2.position.x)
							swap(f1, f2);
						lines.push_back(lerp<Fragment>(f1.position.x, f1, f2.position.x, f2));
					}

					if(settings->textures){
						Vector dx, dy;
						for(unsigned int i = 0; i < lines.size()-1; i+=2){
							auto it1 = lines[i].begin(), it2 = lines[i+1].begin();
							while(it1 < lines[i].end() || it2 < lines[i+1].end()){
								if(it1+1 < lines[i].end())
									dx = (it1+1)->texel/(it1+1)->position.z - it1->texel/it1->position.z;
								else if(it2+1 < lines[i+1].end())
									dx = (it2+1)->texel/(it2+1)->position.z - it2->texel/it2->position.z;

								cout << it1->position.x << " " << it2->position.x << endl;

								if(it1 < lines[i].end() && it2 < lines[i+1].end()){
									if(it1->position.x < it2->position.x)
										dy = it2->texel/it2->position.z - (it1+((int)it2->position.x-(int)it1->position.x))->texel/(it1+((int)it2->position.x-(int)it1->position.x))->position.z;
									else
										dy = (it2+((int)it1->position.x-(int)it2->position.x))->texel/(it2+((int)it1->position.x-(int)it2->position.x))->position.z - it1->texel/it1->position.z;
								}

								if(it1 < lines[i].end() && (it2 >= lines[i+1].end() || (int)it1->position.x <= (int)it2->position.x)){
									it1->texel /= it1->position.z;
									if(it1->material.ambientTexture != nullptr)
										it1->material.ambient *= it1->material.ambientTexture->shade(it1->texel.x, it1->texel.y, dx, dy);
									if(it1->material.diffuseTexture != nullptr)
										it1->material.diffuse *= it1->material.diffuseTexture->shade(it1->texel.x, it1->texel.y, dx, dy);
									if(it1->material.specularTexture != nullptr)
										it1->material.specular *= it1->material.specularTexture->shade(it1->texel.x, it1->texel.y, dx, dy);
									it1++;
								}

								if(it2 < lines[i+1].end() && (it1 >= lines[i].end() || (int)it2->position.x <= (int)it1->position.x)){
									it2->texel /= it2->position.z;
									if(it2->material.ambientTexture != nullptr)
										it2->material.ambient *= it2->material.ambientTexture->shade(it2->texel.x, it2->texel.y, dx, dy);
									if(it2->material.diffuseTexture != nullptr)
										it2->material.diffuse *= it2->material.diffuseTexture->shade(it2->texel.x, it2->texel.y, dx, dy);
									if(it2->material.specularTexture != nullptr)
										it2->material.specular *= it2->material.specularTexture->shade(it2->texel.x, it2->texel.y, dx, dy);
									it2++;
								}
							}
						}
					}
				}

				for(vector<Fragment> line: lines){
					for(Fragment f: line){
						int x = f.position.x, y = f.position.y;

						if(x < 0 || x >= viewPort.width || y < 0 || y >= viewPort.height){
							cout << "out " << x << " " << y << endl;
							throw "pixel drawn out of bounds";
						}

						if(f.position.z >= depthInverse[x][y]){
							depthInverse[x][y] = f.position.z;

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

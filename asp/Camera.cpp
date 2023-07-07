/*
 * Camera.cpp
 *
 *  Created on: Mar 31, 2023
 *      Author: Nathan
 */
#include <cmath>
#include <forward_list>
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

		fragments = new Fragment*[height+2];
		for(int i = 0; i < height+2; i++)
			fragments[i] = new Fragment[width+2];

		clear();

		setFOV(90);
	}

	Camera::~Camera(){
		for(int i = 0; i < viewPort.width; i++)
			delete[] depthInverse[i];
		delete[] depthInverse;

		for(int i = 0; i < viewPort.height+2; i++)
			delete[] fragments[i];
		delete[] fragments;
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
			//cout << "transformed" << endl;

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
			//cout << "culled" << endl;

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

				int dy01 = f1.position.y-f0.position.y;
				int dy02 = f2.position.y-f0.position.y;
				int dy12 = f2.position.y-f1.position.y;

				forward_list<pair<Fragment*, int>> lines;

				Fragment* l01 = lerp<Fragment>(0, f0, dy01, f1);
				Fragment* l02 = lerp<Fragment>(0, f0, dy02, f2, dy02+2);
				Fragment* l12 = lerp<Fragment>(0, f1, dy12, f2, dy12+2);

				if(settings->wireframe){
					f0.normal = Vector3D{0, 0, -1};
					f1.normal = Vector3D{0, 0, -1};
					f2.normal = Vector3D{0, 0, -1};

					lines.push_front(make_pair(l01, dy01));
					lines.push_front(make_pair(l02, dy02));
					lines.push_front(make_pair(l12, dy12));
				}
				else{
					for(int i = 0; i <= dy02; i++){
						Fragment f1 = l02[i];
						Fragment f2 = i < dy01 ? l01[i] : l12[i-dy01];
						if(f1.position.x > f2.position.x)
							swap(f1, f2);

						int start, end;
						if(lines.empty()){
							start = 0;
							end = 1;
						}
						else{
							Fragment fp1 = l02[i-1];
							Fragment fp2 = i-1 < dy01 ? l01[i-1] : l12[i-dy01-1];
							if(fp1.position.x > fp2.position.x)
								swap(fp1, fp2);

							start = min(0.0, fp1.position.x-f1.position.x);
							end = max(1.0, fp2.position.x-f2.position.x);
						}

						lines.push_front(make_pair(
								lerp<Fragment>(fragments[(int)f1.position.y]+(int)f1.position.x,
										f1.position.x, f1,
										f2.position.x, f2,
										f2.position.x-f1.position.x+end-start+1,
										start),
								f2.position.x-f1.position.x+1));
					}
					Fragment f1 = l02[dy02+1];
					Fragment f2 = l12[dy12+1];
					if(f1.position.x > f2.position.x)
						swap(f1, f2);
					Fragment fp1 = l02[dy02];
					Fragment fp2 = l12[dy12];
					if(fp1.position.x > fp2.position.x)
						swap(fp1, fp2);

					lerp<Fragment>(fragments[(int)f1.position.y]+(int)f1.position.x,
							f1.position.x, f1,
							f2.position.x, f2,
							fp2.position.x-fp1.position.x+1,
							fp1.position.x-f1.position.x);

					if(settings->textures){
						for(pair<Fragment*, int> pair: lines){
							for(int i = 0; i < pair.second; i++){
								if(pair.first[i].material.ambientTexture != nullptr)
									pair.first[i].material.ambient *= pair.first[i].material.ambientTexture->shade(fragments, pair.first[i].position.x, pair.first[i].position.y);
								if(pair.first[i].material.diffuseTexture != nullptr)
									pair.first[i].material.diffuse *= pair.first[i].material.diffuseTexture->shade(fragments, pair.first[i].position.x, pair.first[i].position.y);
								if(pair.first[i].material.specularTexture != nullptr)
									pair.first[i].material.specular *= pair.first[i].material.specularTexture->shade(fragments, pair.first[i].position.x, pair.first[i].position.y);
							}
						}
					}
				}

				for(pair<Fragment*, int> pair: lines){
					for(int i = 0; i < pair.second; i++){
						Fragment f = pair.first[i];
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

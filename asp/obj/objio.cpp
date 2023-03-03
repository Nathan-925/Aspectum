/*
 * objio.cpp
 *
 *  Created on: Feb 6, 2023
 *      Author: Nathan
 */
#include <fstream>
#include <vector>
#include <unordered_map>

#include "objio.h"
#include "mtl.h"

#include "priori/Math.h"
#include "priori/Math3D.h"

using namespace std;
using namespace priori;

namespace asp{

	Model readobj(string fileName){
		Model model;
		vector<Point3D> points;
		vector<Point> texels;
		vector<Vector3D> normals;
		unordered_map<string, Material> materials;
		Material activeMaterial;
		ifstream file(fileName);

		while(file.good()){
			string command;
			file >> command;

			if(!(command.empty() || command[0] == '#')){
				if(command.compare("v") == 0){
					points.emplace_back();
					file >> points.back().x >> points.back().y >> points.back().z;
				}
				else if(command.compare("vt") == 0){
					texels.emplace_back();
					file >> texels.back().x >> texels.back().y;
				}
				else if(command.compare("vn") == 0){
					normals.emplace_back();
					file >> normals.back().x >> normals.back().y >> normals.back().z;
				}
				else if(command[0] == 'f'){
					model.triangles.emplace_front();
					int normalIndexes[3];
					for(int i = 0; i < 3; i++){
						string s;
						file >> s;
						uint s1 = s.find('/'), s2 = s.rfind('/');

						model.triangles.front().vertices[i] = new Vertex();

						int pIndex = stoi(s.substr(0, s1));
						model.triangles.front()[i].position = points[pIndex > 0 ? pIndex-1 : points.size()+pIndex];

						if(s1 < s.length()-1 && s2 > s1+1){
							int tIndex = stoi(s.substr(s1+1, s2));
							model.triangles.front()[i].texel = texels[tIndex > 0 ? tIndex-1 : texels.size()+tIndex];
						}

						normalIndexes[i] = s2 < s.length()-1 ? stoi(s.substr(s2+1)) : 0;
					}

					for(int i = 0; i < 3; i++)
						model.triangles.front()[i].normal = normalIndexes[i] == 0 ?
								(model.triangles.front()[0].position-model.triangles.front()[1].position)^(model.triangles.front()[2].position-model.triangles.front()[1].position) :
								normals[normalIndexes[i] > 0 ? normalIndexes[i]-1 : normals.size()+normalIndexes[i]];

					model.triangles.front().material = activeMaterial;

				}
				else if(command.compare("mtllib") == 0){
					string mtlName;
					file >> mtlName;
					auto newMtl = readmtl(mtlName);
					materials.insert(newMtl.begin(), newMtl.end());
				}
				else if(command.compare("usemtl") == 0){
					string mtlName;
					file >> mtlName;
					activeMaterial = materials[mtlName];
				}
			}
			file.ignore(numeric_limits<streamsize>::max(), '\n');
		}

		file.close();
		return model;
	}

}

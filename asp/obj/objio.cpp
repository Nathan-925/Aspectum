/*
 * objio.cpp
 *
 *  Created on: Feb 6, 2023
 *      Author: Nathan
 */
#include <fstream>
#include <vector>
#include <unordered_map>
#include <iostream>

#include "objio.h"
#include "mtl.h"

#include "priori/Math.h"
#include "priori/Math3D.h"

using namespace std;
using namespace priori;

namespace asp{

	Model readobj(string fileName){
		Model model;
		vector<Vector> texels;
		vector<Vector3D> normals;
		string activeMaterial;
		ifstream file(fileName);

		while(file.good()){
			string command;
			file >> command;

			if(!(command.empty() || command[0] == '#')){
				if(command.compare("v") == 0){
					model.vertices.emplace_back();
					file >> model.vertices.back().position.x >> model.vertices.back().position.y >> model.vertices.back().position.z;
					model.vertices.back().position.isPoint = true;
				}
				else if(command.compare("vt") == 0){
					texels.emplace_back();
					file >> texels.back().x >> texels.back().y;
				}
				else if(command.compare("vn") == 0){
					normals.emplace_back();
					file >> normals.back().x >> normals.back().y >> normals.back().z;
					normals.back().isPoint = false;
				}
				else if(command[0] == 'f'){
					model.triangles.emplace_back();
					int normalIndexes[3];
					for(int i = 0; i < 3; i++){
						string s;
						file >> s;
						unsigned int s1 = s.find('/'), s2 = s.rfind('/');

						int pIndex = stoi(s.substr(0, s1));
						model.triangles.back().vertices[i] = pIndex > 0 ? pIndex-1 : model.vertices.size()+pIndex;

						if(s1 < s.length()-1 && s2 > s1+1){
							int tIndex = stoi(s.substr(s1+1, s2));
							model.vertices[model.triangles.back().vertices[i]].texel = texels[tIndex > 0 ? tIndex-1 : texels.size()+tIndex];
						}

						normalIndexes[i] = s2 < s.length()-1 ? stoi(s.substr(s2+1)) : 0;
					}


					Vector3D normal = (model.vertices[model.triangles.back().vertices[0]].position-model.vertices[model.triangles.back().vertices[1]].position)^(model.vertices[model.triangles.back().vertices[2]].position-model.vertices[model.triangles.back().vertices[1]].position);
					for(int i = 0; i < 3; i++)
						model.triangles.back().normals[i] = normalIndexes[i] == 0 ?
								normal :
								normals[normalIndexes[i] > 0 ? normalIndexes[i]-1 : normals.size()+normalIndexes[i]];

					model.triangles.back().material = model.materials[activeMaterial];

				}
				else if(command.compare("mtllib") == 0){
					string mtlName;
					file >> mtlName;
					auto newMtl = readmtl(mtlName);
					model.materials.insert(newMtl.begin(), newMtl.end());
				}
				else if(command.compare("usemtl") == 0){
					file >> activeMaterial;
				}
			}
			file.ignore(numeric_limits<streamsize>::max(), '\n');
		}

		file.close();
		return model;
	}

}

/*
 * ResourceLoader.cpp
 *
 *  Created on: Apr 5, 2023
 *      Author: Nathan
 */
#include <fstream>

#include "ResourceLoader.h"

#include "priori/BMPLibrary.h"

using namespace std;
using namespace priori;

namespace asp{

	ResourceLoader::ResourceLoader(TextureSettings* t) : textureSettings(t), textures(), materials(), models(){}

	void ResourceLoader::readobj(string fileName){
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
							model.triangles.back().texels[i] = texels[tIndex > 0 ? tIndex-1 : texels.size()+tIndex];
						}

						normalIndexes[i] = s2 < s.length()-1 ? stoi(s.substr(s2+1)) : 0;
					}


					Vector3D normal = (model.vertices[model.triangles.back().vertices[0]].position-model.vertices[model.triangles.back().vertices[1]].position)^(model.vertices[model.triangles.back().vertices[2]].position-model.vertices[model.triangles.back().vertices[1]].position);
					for(int i = 0; i < 3; i++)
						model.triangles.back().normals[i] = normalIndexes[i] == 0 ?
								normal :
								normals[normalIndexes[i] > 0 ? normalIndexes[i]-1 : normals.size()+normalIndexes[i]];

					model.triangles.back().material = materials[activeMaterial];

				}
				else if(command.compare("mtllib") == 0){
					string mtlName;
					file >> mtlName;
					readmtl(mtlName);
				}
				else if(command.compare("usemtl") == 0){
					file >> activeMaterial;
				}
			}
			file.ignore(numeric_limits<streamsize>::max(), '\n');
		}

		file.close();
		models.emplace(fileName, model);
	}

	void ResourceLoader::readmtl(string fileName){
		ifstream file(fileName);
		string active;

		while(file.good()){
			string command;
			file >> command;

			if(!(command.empty() || command[0] == '#')){
				if(command.compare("newmtl") == 0){
					file >> active;
					materials.emplace(active, Material{0xFFFFFF, 0xFFFFFF, 0xFFFFFF,
													   nullptr, nullptr, nullptr,
													   1, -1, 1});
				}
				else if(command[0] == 'K'){
					double r, g, b;
					file >> r >> g >> b;
					if(command[1] == 'a')
						materials[active].ambient = Color(255*r, 255*g, 255*b);
					else if(command[1] == 'd')
						materials[active].diffuse = Color(255*r, 255*g, 255*b);
					else if(command[1] == 's')
						materials[active].specular = Color(255*r, 255*g, 255*b);
				}
				else if(command.compare("illum") == 0){
					file >> materials[active].illuminationModel;
					materials[active].illuminationModel = min(2, materials[active].illuminationModel);
				}
				else if(command.compare("Ns") == 0){
					file >> materials[active].shine;
				}
				else if(command.substr(0, 3).compare("map") == 0){
					string textureFile;
					file >> textureFile;
					if(textureFile.substr(textureFile.length()-3).compare("bmp") != 0){
						throw "non bmp texture file";
					}

					Texture t(readbmp(textureFile), textureSettings);
					textures.emplace(textureFile, t);
					string destination = command.substr(4);
					if(destination.compare("Ka") == 0)
						materials[active].ambientTexture = &textures.at(textureFile);
					else if(destination.compare("Kd") == 0)
						materials[active].diffuseTexture = &textures.at(textureFile);
					else if(destination.compare("Ks") == 0)
						materials[active].specularTexture = &textures.at(textureFile);
				}
			}
			file.ignore(numeric_limits<streamsize>::max(), '\n');
		}

		file.close();
	}

	void ResourceLoader::addTexture(string name, Image image){
		textures.emplace(name, Texture(image, textureSettings));
	}

}

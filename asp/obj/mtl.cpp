/*
 * mtl.cpp
 *
 *  Created on: Feb 7, 2023
 *      Author: Nathan
 */
#include <fstream>

#include "mtl.h"

#include "priori/BMPLibrary.h"

using namespace std;
using namespace priori;


namespace asp{

	unordered_map<string, Material> readmtl(string fileName){
		unordered_map<string, Material> materials;
		ifstream file(fileName);
		string active;

		while(file.good()){
			string command;
			file >> command;

			if(!(command.empty() || command[0] == '#')){
				if(command.compare("newmtl") == 0){
					file >> active;
					materials.insert(make_pair(active, Material{0xFFFFFF, 0xFFFFFF, 0xFFFFFF,
																nullptr, nullptr, nullptr,
																1, -1, 1}));
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
					if(textureFile.substr(textureFile.length()-3).compare("bmp") != 0)
						throw "non bmp texture file";

					Texture* texture = new Texture(readbmp(textureFile));
					string destination = command.substr(4);
					if(destination.compare("Ka") == 0)
						materials[active].ambientTexture = texture;
					else if(destination.compare("Kd") == 0)
						materials[active].diffuseTexture = texture;
					else if(destination.compare("Ks") == 0)
						materials[active].specularTexture = texture;
					else
						delete texture;
				}
			}
			file.ignore(numeric_limits<streamsize>::max(), '\n');
		}

		file.close();
		return materials;
	}

}

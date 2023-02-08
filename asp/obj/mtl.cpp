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

	unordered_map<std::string, Material> readmtl(string fileName){
		unordered_map<std::string, Material> materials;
		ifstream file(fileName);
		string active;

		while(file.good()){
			string command;
			file >> command;

			if(command.compare("newmtl") == 0){
				file >> active;
				materials.insert(make_pair(active, Material()));
			}
			else if(command.compare("Ka") == 0){
				file >> materials[active].ambient.r >> materials[active].ambient.g >> materials[active].ambient.b;
			}
			else if(command.compare("Kd") == 0){
				file >> materials[active].diffuse.r >> materials[active].diffuse.g >> materials[active].diffuse.b;
			}
			else if(command.compare("Ks") == 0){
				file >> materials[active].specular.r >> materials[active].specular.g >> materials[active].specular.b;
			}
			else if(command.compare("illum") == 0){
				file >> materials[active].illuminationModel;
			}
			else if(command.compare("Ns") == 0){
				file >> materials[active].shine;
			}
			else if(command.substr(0, 3).compare("map") == 0){
				string textureFile;
				file >> textureFile;
				if(textureFile.substr(textureFile.length()-3).compare("bmp") != 0)
					throw "non bmp texture file";


				Texture texture = Texture(readbmp(textureFile));
				string destination = command.substr(4);
				if(destination.compare("Ka") == 0)
					materials[active].ambientTexture = &texture;
				else if(destination.compare("Kd") == 0)
					materials[active].diffuseTexture = &texture;
				else if(destination.compare("Ks") == 0)
					materials[active].specularTexture = &texture;
			}

			file.ignore(numeric_limits<streamsize>::max(), '\n');
		}

		file.close();
		return materials;
	}

}

/*
 * ResorceLoader.h
 *
 *  Created on: Apr 5, 2023
 *      Author: Nathan
 */

#ifndef OBJ_RESOURCELOADER_H_
#define OBJ_RESOURCELOADER_H_

#include <unordered_map>
#include <string>

#include "Model.h"

namespace asp{

	class ResourceLoader{
	public:
		TextureSettings* textureSettings;

		std::unordered_map<std::string, Texture> textures;
		std::unordered_map<std::string, Material> materials;
		std::unordered_map<std::string, Model> models;

		ResourceLoader(TextureSettings* t = nullptr);

		void readobj(std::string filename);
		void readmtl(std::string fileName);
		void addTexture(std::string name, priori::Image image);
		void clear();
	};

}

#endif /* OBJ_RESOURCELOADER_H_ */

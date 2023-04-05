/*
 * mtl.h
 *
 *  Created on: Feb 7, 2023
 *      Author: Nathan
 */

#ifndef OBJ_MTL_H_
#define OBJ_MTL_H_

#include <string>
#include <unordered_map>

#include "asp/Model.h"

#include "priori/Graphical.h"

namespace asp{

	void readmtl(std::string fileName, Model &model, TextureSettings* textureSettings);
	void writemtl(std::string fileName, std::unordered_map<std::string, Material> materials);

}

#endif /* OBJ_MTL_H_ */

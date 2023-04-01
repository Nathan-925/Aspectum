/*
 * Projection.h
 *
 *  Created on: Dec 9, 2022
 *      Author: Nathan
 */

#ifndef SCENE_H_
#define SCENE_H_

#include "Model.h"
#include "Lights.h"

namespace asp{

	struct Scene{
		std::vector<Instance*> objects;
		std::vector<Light*> lights;
	};

}

#endif /* SCENE_H_ */

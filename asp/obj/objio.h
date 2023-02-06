/*
 * objio.h
 *
 *  Created on: Feb 6, 2023
 *      Author: Nathan
 */

#ifndef OBJ_OBJIO_H_
#define OBJ_OBJIO_H_

#include <string>

#include "asp/Projection.h"

namespace asp{
	void writeobj(std::string fileName, Model model);
	Model readobj(std::string fileName);
}

#endif /* OBJ_OBJIO_H_ */

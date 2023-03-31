/*
 * Shaders.cpp
 *
 *  Created on: Mar 29, 2023
 *      Author: Nathan
 */
#include "Shaders.h"

using namespace std;
using namespace priori;

namespace asp{

	void colorNormals(Fragment &fragment){
		Vector3D normal = fragment.normal.normalize()*0.5;
		fragment.color = fragment.normal.z > 0 ? 0 : Color((normal.x+0.5)*255, (normal.y+0.5)*255, (-normal.z+0.5)*255);
	}

}

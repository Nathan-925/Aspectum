/*
 * Shaders.cpp
 *
 *  Created on: Mar 29, 2023
 *      Author: Nathan
 */
#include <iostream>

#include "Shaders.h"

using namespace std;
using namespace priori;

namespace asp{

	void colorNormals(Fragment &fragment){
		Vector3D normal = fragment.normal.normalize();
		if(normal.z > 0)
			cout << normal.x << " " << normal.y << " " << normal.z << endl;
		fragment.color = fragment.normal.z > 00 ? 0 : Color(abs(normal.x)*255, abs(normal.y)*255, abs(normal.z)*255);
	}

}

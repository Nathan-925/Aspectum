/*
 * Shapes.cpp
 *
 *  Created on: Dec 9, 2022
 *      Author: Nathan
 */
#include <iostream>

#include "Shapes.h"

#include "priori/Math.h"
#include "priori/Math3D.h"

using namespace std;
using namespace priori;

namespace asp{
	Model getCube(Texture* t){
		Model cube;

		Point3D v[8];
		for(int i = 0; i < 8; i++)
			v[i] = Point3D(i%2, (i>>1)%2, (i>>2)%2);

		cube.emplace_front();
		cube.front()[0].position = v[0];
		cube.front()[1].position = v[1];
		cube.front()[2].position = v[3];
		cube.front().normals[0] = (cube.front()[0].position-cube.front()[1].position)^(cube.front()[2].position-cube.front()[1].position);
		cube.front().normals[1] = cube.front().normals[0];
		cube.front().normals[2] = cube.front().normals[0];
		cube.front().texture = t;

		cube.emplace_front();
		cube.front()[0].position = v[0];
		cube.front()[1].position = v[3];
		cube.front()[2].position = v[2];
		cube.front().normals[0] = (cube.front()[0].position-cube.front()[1].position)^(cube.front()[2].position-cube.front()[1].position);
		cube.front().normals[1] = cube.front().normals[0];
		cube.front().normals[2] = cube.front().normals[0];
		cube.front().texture = t;

		cube.emplace_front();
		cube.front()[0].position = v[4];
		cube.front()[1].position = v[5];
		cube.front()[2].position = v[1];
		cube.front().normals[0] = (cube.front()[0].position-cube.front()[1].position)^(cube.front()[2].position-cube.front()[1].position);
		cube.front().normals[1] = cube.front().normals[0];
		cube.front().normals[2] = cube.front().normals[0];
		cube.front().texture = t;

		cube.emplace_front();
		cube.front()[0].position = v[4];
		cube.front()[1].position = v[1];
		cube.front()[2].position = v[0];
		cube.front().normals[0] = (cube.front()[0].position-cube.front()[1].position)^(cube.front()[2].position-cube.front()[1].position);
		cube.front().normals[1] = cube.front().normals[0];
		cube.front().normals[2] = cube.front().normals[0];
		cube.front().texture = t;

		cube.emplace_front();
		cube.front()[0].position = v[5];
		cube.front()[1].position = v[7];
		cube.front()[2].position = v[3];
		cube.front().normals[0] = (cube.front()[0].position-cube.front()[1].position)^(cube.front()[2].position-cube.front()[1].position);
		cube.front().normals[1] = cube.front().normals[0];
		cube.front().normals[2] = cube.front().normals[0];
		cube.front().texture = t;

		cube.emplace_front();
		cube.front()[0].position = v[5];
		cube.front()[1].position = v[3];
		cube.front()[2].position = v[1];
		cube.front().normals[0] = (cube.front()[0].position-cube.front()[1].position)^(cube.front()[2].position-cube.front()[1].position);
		cube.front().normals[1] = cube.front().normals[0];
		cube.front().normals[2] = cube.front().normals[0];
		cube.front().texture = t;

		cube.emplace_front();
		cube.front()[0].position = v[7];
		cube.front()[1].position = v[6];
		cube.front()[2].position = v[2];
		cube.front().normals[0] = (cube.front()[0].position-cube.front()[1].position)^(cube.front()[2].position-cube.front()[1].position);
		cube.front().normals[1] = cube.front().normals[0];
		cube.front().normals[2] = cube.front().normals[0];
		cube.front().texture = t;

		cube.emplace_front();
		cube.front()[0].position = v[7];
		cube.front()[1].position = v[2];
		cube.front()[2].position = v[3];
		cube.front().normals[0] = (cube.front()[0].position-cube.front()[1].position)^(cube.front()[2].position-cube.front()[1].position);
		cube.front().normals[1] = cube.front().normals[0];
		cube.front().normals[2] = cube.front().normals[0];
		cube.front().texture = t;

		cube.emplace_front();
		cube.front()[0].position = v[6];
		cube.front()[1].position = v[4];
		cube.front()[2].position = v[0];
		cube.front().normals[0] = (cube.front()[0].position-cube.front()[1].position)^(cube.front()[2].position-cube.front()[1].position);
		cube.front().normals[1] = cube.front().normals[0];
		cube.front().normals[2] = cube.front().normals[0];
		cube.front().texture = t;

		cube.emplace_front();
		cube.front()[0].position = v[6];
		cube.front()[1].position = v[0];
		cube.front()[2].position = v[2];
		cube.front().normals[0] = (cube.front()[0].position-cube.front()[1].position)^(cube.front()[2].position-cube.front()[1].position);
		cube.front().normals[1] = cube.front().normals[0];
		cube.front().normals[2] = cube.front().normals[0];
		cube.front().texture = t;

		cube.emplace_front();
		cube.front()[0].position = v[4];
		cube.front()[1].position = v[7];
		cube.front()[2].position = v[5];
		cube.front().normals[0] = (cube.front()[0].position-cube.front()[1].position)^(cube.front()[2].position-cube.front()[1].position);
		cube.front().normals[1] = cube.front().normals[0];
		cube.front().normals[2] = cube.front().normals[0];
		cube.front().texture = t;

		cube.emplace_front();
		cube.front()[0].position = v[4];
		cube.front()[1].position = v[6];
		cube.front()[2].position = v[7];
		cube.front().normals[0] = (cube.front()[0].position-cube.front()[1].position)^(cube.front()[2].position-cube.front()[1].position);
		cube.front().normals[1] = cube.front().normals[0];
		cube.front().normals[2] = cube.front().normals[0];
		cube.front().texture = t;

		for(auto it = cube.begin(); it != cube.end(); it++){
			cout << (*it).normals[0].x << " " << (*it).normals[0].y << " " << (*it).normals[0].z << endl;
			(*it)[0].texel = Point(0, 1);
			(*it)[1].texel = Point(1, 0);
			(*it)[2].texel = Point(0, 0);
            it++;

			(*it)[0].texel = Point(0, 1);
			(*it)[1].texel = Point(1, 1);
			(*it)[2].texel = Point(1, 0);
		}

		return cube;
	}
}

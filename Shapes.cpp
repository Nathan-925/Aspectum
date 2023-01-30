/*
 * Shapes.cpp
 *
 *  Created on: Dec 9, 2022
 *      Author: Nathan
 */
#include "Shapes.h"
#include "priori/Math.h"
#include "priori/Math3D.h"

using namespace priori;

namespace asp{
	Model getCube(Texture* t){
		Model cube;

		Vertex v[8];
		for(int i = 0; i < 8; i++)
			v[i].position = Point3D(i%2, (i>>1)%2, (i>>2)%2);

		cube.emplace_front();
		cube.front()[0] = v[0];
		cube.front()[1] = v[1];
		cube.front()[2] = v[3];
		cube.front().texture = t;

		cube.emplace_front();
		cube.front()[0] = v[0];
		cube.front()[1] = v[2];
		cube.front()[2] = v[3];
		cube.front().texture = t;

		cube.emplace_front();
		cube.front()[0] = v[0];
		cube.front()[1] = v[1];
		cube.front()[2] = v[5];
		cube.front().texture = t;

		cube.emplace_front();
		cube.front()[0] = v[0];
		cube.front()[1] = v[4];
		cube.front()[2] = v[5];
		cube.front().texture = t;

		cube.emplace_front();
		cube.front()[0] = v[1];
		cube.front()[1] = v[3];
		cube.front()[2] = v[7];
		cube.front().texture = t;

		cube.emplace_front();
		cube.front()[0] = v[1];
		cube.front()[1] = v[5];
		cube.front()[2] = v[7];
		cube.front().texture = t;

		cube.emplace_front();
		cube.front()[0] = v[3];
		cube.front()[1] = v[2];
		cube.front()[2] = v[6];
		cube.front().texture = t;

		cube.emplace_front();
		cube.front()[0] = v[3];
		cube.front()[1] = v[7];
		cube.front()[2] = v[6];
		cube.front().texture = t;

		cube.emplace_front();
		cube.front()[0] = v[2];
		cube.front()[1] = v[0];
		cube.front()[2] = v[4];
		cube.front().texture = t;

		cube.emplace_front();
		cube.front()[0] = v[2];
		cube.front()[1] = v[6];
		cube.front()[2] = v[4];
		cube.front().texture = t;

		cube.emplace_front();
		cube.front()[0] = v[4];
		cube.front()[1] = v[5];
		cube.front()[2] = v[7];
		cube.front().texture = t;

		cube.emplace_front();
		cube.front()[0] = v[4];
		cube.front()[1] = v[6];
		cube.front()[2] = v[7];
		cube.front().texture = t;

		for(auto it = cube.begin(); it != cube.end(); it++){
			(*it)[0].texel = Point(0, 0);
			(*it)[1].texel = Point(1, 0);
			(*it)[2].texel = Point(1, 1);
            it++;

			(*it)[0].texel = Point(0, 0);
			(*it)[1].texel = Point(0, 1);
			(*it)[2].texel = Point(1, 1);
		}

		return cube;
	}
}

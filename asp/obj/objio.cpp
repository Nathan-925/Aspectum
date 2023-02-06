/*
 * objio.cpp
 *
 *  Created on: Feb 6, 2023
 *      Author: Nathan
 */
#include <fstream>
#include <vector>

#include "objio.h"

using namespace std;

namespace asp{

	Model readobj(string fileName){
		Model out;
		vector<Vertex> vertices;
		ifstream file(fileName);

		while(!file.eof()){
			switch(file.get()){
			case 'v':
				double x, y, z;
				file >> x >> y >> z;
				vertices.emplace_back(x, y, z);
				file.getline();
			}
		}
	}

}

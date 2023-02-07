/*
 * objio.cpp
 *
 *  Created on: Feb 6, 2023
 *      Author: Nathan
 */
#include <fstream>
#include <vector>

#include "objio.h"

#include "priori/Math.h"
#include "priori/Math3D.h"

using namespace std;
using namespace priori;

template<class T>
T& getAbsolutePosition(vector<T> vector, int n){
	if(n < 0)
		return vector[vector.size()+n];
	return vector[n-1];
}

namespace asp{

	Model readobj(string fileName){
		Model model;
		vector<Point3D> points;
		vector<Point> texels;
		vector<Vector3D> normals;
		ifstream file(fileName);

		while(file.good()){
			string command;
			file >> command;

			if(command[0] == 'v'){
				double x, y, z;
				file >> x;
				if(file.peek() == '\n'){
					y = 0;
					z = 0;
				}
				else{
					file >> y;
					if(file.peek() == '\n')
						z = 0;
					else
						file >> z;
				}
				if(command[1] == 't')
					texels.emplace_back(x, y);
				else if(command[1] == 'n')
					normals.emplace_back(x, y, z);
				else
					points.emplace_back(x, y, z);
			}
			else if(command[0] == 'f'){
				model.emplace_front();
				int indexes[3][3];
				for(int i = 0; i < 3; i++){
					string s;
					file >> s;
					int s1 = s.find('/'), s2 = s.rfind('/');
					model.front()[0].position = stoi(s.substr(0, s1))-1;
					if(s1 != s.npos && s2 > s1+1)
						model.front()[0].texel = stoi(s.substr(s1+1, s2));
					indexes[i][2] = s2 < s.npos-1 ? stoi(s.substr(s2+1)) : 0;
				}


			}
			file.ignore(numeric_limits<streamsize>::max(), '\n');
		}
	}

}

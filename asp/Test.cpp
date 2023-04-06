/*
 * Test.cpp
 *
 *  Created on: Apr 6, 2023
 *      Author: Nathan
 */
#include <iostream>
#include <unordered_map>
#include <string>

#include "Model.h"

#include "priori/Graphical.h"
#include "priori/BMPLibrary.h"

using namespace std;
using namespace priori;
using namespace asp;

int main(){
	unordered_map<string, Texture> t;
	Image img = readbmp("mario/mario_logo.bmp");
	TextureSettings s;
	Texture tex(img, &s);
	t.emplace("test", tex);
}

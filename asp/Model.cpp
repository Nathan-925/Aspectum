/*
 * Model.cpp
 *
 *  Created on: Mar 31, 2023
 *      Author: Nathan
 */
#include <iostream>

#include "Model.h"

using namespace std;
using namespace priori;

namespace asp{

	priori::Image* debug;

	Vertex Vertex::operator+(Vertex other) const{
		other.position += position;
		other.texel += texel;
		return other;
	}

	Vertex Vertex::operator-(Vertex other) const{
		other.position = position-other.position;
		other.texel = texel-other.texel;
		return other;
	}

	Vertex Vertex::operator+=(const Vertex &other){
		position += other.position;
		texel += other.texel;
		return *this;
	}

	Vertex Vertex::operator-=(const Vertex &other){
		position -= other.position;
		texel -= other.texel;
		return *this;
	}

	Vertex Vertex::operator*(const double &d) const{
		Vertex out(*this);
		out.position *= d;
		out.texel *= d;
		return out;
	}

	Vertex Vertex::operator/(const double &d) const{
		Vertex out(*this);
		out.position /= d;
		out.texel /= d;
		return out;
	}

	Vertex Vertex::operator*=(const double &d){
		position *= d;
		texel *= d;
		return *this;
	}

	Vertex Vertex::operator/=(const double &d){
		position /= d;
		texel /= d;
		return *this;
	}

	Vertex operator*(const TransformationMatrix &transform, Vertex vertex){
		vertex.position = transform*vertex.position;
		return vertex;
	}

	Vertex operator*=(Vertex &vertex, const priori::TransformationMatrix &transform){
		vertex.position = transform*vertex.position;
		return vertex;
	}

	Fragment Fragment::operator+(Fragment other) const{
		other.position += position;
		other.texel += texel;
		other.normal += normal;
		return other;
	}

	Fragment Fragment::operator-(Fragment other) const{
		other.position = position-other.position;
		other.texel = texel-other.texel;
		other.normal = normal-other.normal;
		return other;
	}

	Fragment Fragment::operator+=(const Fragment &other){
		position += other.position;
		texel += other.texel;
		normal += other.normal;
		return *this;
	}

	Fragment Fragment::operator-=(const Fragment &other){
		position -= other.position;
		texel -= other.texel;
		normal -= other.normal;
		return *this;
	}

	Fragment Fragment::operator*(const double &d) const{
		Fragment out(*this);
		out.position *= d;
		out.texel *= d;
		out.normal *= d;
		return out;
	}

	Fragment Fragment::operator/(const double &d) const{
		Fragment out(*this);
		out.position /= d;
		out.texel /= d;
		out.normal /= d;
		return out;
	}

	Fragment Fragment::operator*=(const double &d){
		position *= d;
		texel *= d;
		normal *= d;
		return *this;
	}

	Fragment Fragment::operator/=(const double &d){
		position /= d;
		texel /= d;
		normal /= d;
		return *this;
	}

	Texture::Texture(Image image, TextureSettings* settings) : settings(settings){
		if((image.height&(image.height-1)) != 0 || (image.width&(image.width-1)) != 0){
			cout << "Texture is incorrect size " << image.width << " " << image.height << endl;
			throw "Texture is incorrect size";
		}

		images.push_back(image);
		for(int i = 0; i < image.width; i++)
			for(int j = 0; j < image.height; j++){
				if(settings->correctGamma){
					images.back()[i][j].r = 255*pow(image[i][j].r/255.0, 2.2);
					images.back()[i][j].g = 255*pow(image[i][j].g/255.0, 2.2);
					images.back()[i][j].b = 255*pow(image[i][j].b/255.0, 2.2);
				}
				else{
					images.back()[i][j].r = 255*(image[i][j].r/255.0);
					images.back()[i][j].g = 255*(image[i][j].g/255.0);
					images.back()[i][j].b = 255*(image[i][j].b/255.0);
				}
			}

		while(min(images.back().width, images.back().height) > 2){
			Image prev = images.back();
			images.emplace_back(prev.width/2, prev.height/2);
			for(int i = 0; i < images.back().width; i++)
				for(int j = 0; j < images.back().height; j++)
					images.back()[i][j] = average(average(prev[i*2][j*2], prev[i*2+1][j*2]),
												  average(prev[i*2][j*2], prev[i*2+1][j*2]));
		}

		//Color c[] = {0xFF, 0xFF00, 0xFF0000, 0xFFFF, 0xFFFF00, 0xFF00FF, 0xBBBBBB};
		//for(unsigned int i = 0; i < images.size(); i++)
		//	for(int j = 0; j < images[i].width; j++)
		//		for(int l = 0; l < images[i].height; l++)
		// 			images[i][j][l] = c[i%(sizeof(c)/sizeof(Color))]/(i/(sizeof(c)/sizeof(Color))+1);
	}

	Color Texture::bilinear(int layer, double x, double y){
		double xPos, yPos;
		double xFrac = modf(x*images[layer].width, &xPos), yFrac = modf(images[layer].height-y*images[layer].height, &yPos);
		if(xPos >= images[layer].width-1){
			xPos = images[layer].width-2;
			xFrac = 1;
		}
		if(yPos >= images[layer].height-1){
			yPos = images[layer].height-2;
			yFrac = 1;
		}
		//cout << x << " " << y << " " << xPos << " " << yPos << " " << layer << " " << images[layer].width << endl;
		return lerp<Color>(yFrac,
				lerp<Color>(xFrac, images[layer][(int)xPos][(int)yPos], images[layer][(int)xPos+1][(int)yPos]),
				lerp<Color>(xFrac, images[layer][(int)xPos][(int)yPos+1], images[layer][(int)xPos+1][(int)yPos+1]));
	}

	Color Texture::shade(Fragment** fragment, int x, int y){
		//int c = 0;
		//if(fragment[y][x].material.alpha == 0.6)
		//	c |= 0xFF;
		//if(fragment[y+1][x].material.alpha == 0.6)
		//	c |=  0xFF00;
		//if(fragment[y][x+1].material.alpha == 0.6)
		//	c |=  0xFF0000;
		//return Color(c)*0.8;

		Vector texel = fragment[y][x].texel/fragment[y][x].position.z;
		double baseMap = 0;

		if(settings->mipmapping){
			Vector dx = fragment[y][x+1].texel/fragment[y][x+1].position.z-texel;
			Vector dy = fragment[y+1][x].texel/fragment[y+1][x].position.z-texel;

			double nx = dx*dx, ny = dy*dy;
			double p = sqrt(max(nx, ny));

			//return Color(255*texel.x, 0, 255*texel.y);
			//return Color(255*(fragment[y][x+1].texel.x/fragment[y][x+1].position.z), 0, 255*(fragment[y+1][x].texel.y/fragment[y+1][x].position.z));
			//return Color(255*sqrt(nx), 0, 255*sqrt(ny));

			if(p == 0)
				p = 1;
			baseMap = max(0.0, images.size()+min(-1.0, log2(p)));
		}

		double xPos, yPos;
		if(settings->wrap){
			double temp;
			xPos = modf(texel.x, &temp);
			if(xPos < 0)
				xPos += 1;
			yPos = modf(texel.y, &temp);
			if(yPos < 0)
				yPos += 1;
		}
		else{
			xPos = min(1.0, max(0.0, texel.x));
			yPos = min(1.0, max(0.0, texel.y));
		}

		if(settings->filtering == settings->TRILINEAR && settings->mipmapping && images.size() > 1 && min(images[baseMap].width, images[baseMap].height) >= 4){
			double mapFrac = modf(baseMap, &baseMap);
			if(baseMap >= images.size()-1){
				baseMap = images.size()-2;
				mapFrac = 1;
			}

			return lerp<Color>(mapFrac, bilinear(baseMap, xPos, yPos), bilinear(baseMap+1, xPos, yPos));
		}

		if(settings->filtering == settings->BILINEAR && min(images[baseMap].width, images[baseMap].height) > 1)
			return bilinear(baseMap, xPos, yPos);

		return images[(int)baseMap][(int)(xPos*(images[baseMap].width-1))][images[baseMap].height-1-(int)(yPos*(images[baseMap].height-1))];
	}

	Instance::Instance(Model* m) : model(m), transform() {};

	Instance::Instance(Model* m, TransformationMatrix t) : model(m), transform(t) {};

}

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

	Vertex Vertex::operator+(Vertex other) const{
		other.position += position;
		return other;
	}

	Vertex Vertex::operator-(Vertex other) const{
		other.position = position-other.position;
		return other;
	}

	Vertex Vertex::operator+=(const Vertex &other){
		position += other.position;
		return *this;
	}

	Vertex Vertex::operator-=(const Vertex &other){
		position -= other.position;
		return *this;
	}

	Vertex Vertex::operator*(const double &d) const{
		Vertex out(*this);
		out.position *= d;
		return out;
	}

	Vertex Vertex::operator/(const double &d) const{
		Vertex out(*this);
		out.position /= d;
		return out;
	}

	Vertex Vertex::operator*=(const double &d){
		position *= d;
		return *this;
	}

	Vertex Vertex::operator/=(const double &d){
		position /= d;
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

		Color c[] = {0xFF, 0xFF00, 0xFF0000, 0xFFFFFF, 0xFFFF, 0xFFFF00, 0xFF00FF};
		for(int i = 0; i < images.size(); i++)
			for(int j = 0; j < images[i].width; j++)
				for(int l = 0; l < images[i].height; l++)
					images[i][j][l] = c[i&7];
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

	Color Texture::shade(Fragment** fragment){
		//cout << settings->mipmapping << endl;
		double baseMap = 0;
		//cout << 1/dy.y << " " << p << " " << log2(p) << " " << baseMap << " " << images[baseMap].width << " " << images.size() << endl;
		//cout << baseMap << " " << images.size() << endl;

		if(settings->mipmapping){
			Vector dx = fragment[1][0].texel-fragment[0][0].texel;
			Vector dy = fragment[0][1].texel-fragment[0][0].texel;
			double p = sqrt(max(dx.x*dx.x+dx.y*dx.y, dy.x*dy.x+dy.y*dy.y));
			baseMap = max(0.0, images.size()+min(-1.0, log2(p)));
		}

		int x, y;
		if(settings->wrap){
			double temp;
			x = modf(fragment[0][0].position.x, &temp);
			if(x < 0)
				x += 1;
			y = modf(fragment[0][0].position.y, &temp);
			if(y < 0)
				y += 1;
		}
		else{
			x = min(1.0, max(0.0, fragment[0][0].position.x));
			y = min(1.0, max(0.0, fragment[0][0].position.y));
		}

		if(settings->filtering == settings->TRILINEAR && settings->mipmapping && images.size() > 1 && min(images[baseMap].width, images[baseMap].height) >= 4){
			double mapFrac = modf(baseMap, &baseMap);
			if(baseMap >= images.size()-1){
				baseMap = images.size()-2;
				mapFrac = 1;
			}
			//cout << baseMap << " " << mapFrac << " " << images.size() << endl;
			return lerp<Color>(mapFrac, bilinear(baseMap, x, y), bilinear(baseMap+1, x, y));
		}

		if(settings->filtering == settings->BILINEAR && min(images[baseMap].width, images[baseMap].height) > 1)
			return bilinear(baseMap, x, y);

		return images[baseMap][(int)round(x*(images[baseMap].width-1))][images[baseMap].height-1-(int)round(y*(images[baseMap].height-1))];
	}

	Instance::Instance(Model* m) : model(m), transform() {};

	Instance::Instance(Model* m, TransformationMatrix t) : model(m), transform(t) {};

}

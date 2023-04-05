/*
 * Model.cpp
 *
 *  Created on: Mar 31, 2023
 *      Author: Nathan
 */
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
		if(image.width != image.height || (image.width&(image.width-1)) != 0)
			throw "Texture is incorrect size";

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

		while(images.back().width > 1){
			Image prev = images.back();
			images.emplace_back(prev.width/2, prev.height);
			for(int i = 0; i < images.back().width; i++)
				for(int j = 0; j < images.back().height; j++)
					images.back()[i][j] = average(average(prev[i*2][j*2], prev[i*2+1][j*2]),
												  average(prev[i*2][j*2], prev[i*2+1][j*2]));
		}
	}

	Color Texture::shade(double x, double y, int width){
		int baseMap = log2(width);
		if(settings->wrap){
			double temp;
			x = modf(x, &temp);
			if(x < 0)
				x += 1;
			y = modf(y, &temp);
			if(y < 0)
				y += 1;
		}
		else{
			x = min(1.0, max(0.0, x));
			y = min(1.0, max(0.0, y));
		}
		return images[baseMap][(int)round(x*(images[baseMap].width-1))][images[baseMap].height-1-(int)round(y*(images[baseMap].height-1))];
	}

	Instance::Instance(Model* m) : model(m), transform() {};

	Instance::Instance(Model* m, TransformationMatrix t) : model(m), transform(t) {};

}

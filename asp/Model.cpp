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

	Texture::Texture(Image image, bool correctGamma) : image(image) {
		if(correctGamma)
			for(int i = 0; i < image.width; i++)
				for(int j = 0; j < image.height; j++){
					this->image[i][j].r = 255*pow(image[i][j].r/255.0, 2.2);
					this->image[i][j].g = 255*pow(image[i][j].g/255.0, 2.2);
					this->image[i][j].b = 255*pow(image[i][j].b/255.0, 2.2);
				}
	}

	Color Texture::getColor(double x, double y){
		x = min(1.0, max(0.0, x));
		y = min(1.0, max(0.0, y));
		return image.pixels[(int)round(x*(image.width-1))][image.height-1-(int)round(y*(image.height-1))];
	}

	Instance::Instance(Model* m) : model(m), transform() {};

	Instance::Instance(Model* m, TransformationMatrix t) : model(m), transform(t) {};

}

/*
 * 3DMath.cpp
 *
 *  Created on: Dec 7, 2022
 *      Author: Nathan
 */
#include "Math3D.h"

#include <cmath>

namespace math3d{

	Point3D Point3D::transform(const TransformationMatrix &matrix){
		long double arr[3];
		for(int i = 0; i < 3; i++){
			arr[i] = 0;
			for(int j = 0; j < 4; j++)
				arr[i] += matrix.matrix[j][i]*mat[j];
		}
		return Point3D(arr[0], arr[1], arr[2]);
	}

	Point3D Point3D::operator+(const Point3D &other){
		return Point3D(x+other.x, y+other.y, z+other.z);
	}

	Point3D Point3D::operator-(const Point3D &other){
		return Point3D(x-other.x, y-other.y, z-other.z);
	}

	Point3D Point3D::operator*(const Point3D &other){
		return Point3D(x*other.x, y*other.y, z*other.z);
	}

	Point3D Point3D::operator/(const Point3D &other){
		return Point3D(x/other.x, y/other.y, z/other.z);
	}

	Point3D Point3D::operator+=(const Point3D &other){
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}

	Point3D Point3D::operator-=(const Point3D &other){
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
	}

	Point3D Point3D::operator*=(const Point3D &other){
		x *= other.x;
		y *= other.y;
		z *= other.z;
		return *this;
	}

	Point3D Point3D::operator/=(const Point3D &other){
		x /= other.x;
		y /= other.y;
		z /= other.z;
		return *this;
	}

	Point3D Point3D::operator+(const double &other){
		return Point3D(x+other, y+other, z+other);
	}

	Point3D Point3D::operator-(const double &other){
		return Point3D(x-other, y-other, z-other);
	}

	Point3D Point3D::operator*(const double &other){
		return Point3D(x*other, y*other, z*other);
	}

	Point3D Point3D::operator/(const double &other){
		return Point3D(x/other, y/other, z/other);
	}

	Point3D Point3D::operator+=(const double &other){
		x += other;
		y += other;
		z += other;
		return *this;
	}

	Point3D Point3D::operator-=(const double &other){
		x -= other;
		y -= other;
		z -= other;
		return *this;
	}

	Point3D Point3D::operator*=(const double &other){
		x *= other;
		y *= other;
		z *= other;
		return *this;
	}

	Point3D Point3D::operator/=(const double &other){
		x /= other;
		y /= other;
		z /= other;
		return *this;
	}

	TransformationMatrix TransformationMatrix::operator*(const TransformationMatrix &other){
		TransformationMatrix out;
		for(int i = 0; i < 4; i++){
			for(int j = 0; j < 4; j++){
				out.matrix[i][j] = 0;
				for(int l = 0; l < 4; l++){
					out.matrix[i][j] += matrix[i][l]*other.matrix[l][j];
				}
			}
		}
		return out;
	}

	double distanceToUnitPlane(Point3D point, Plane plane){
		return point.x*plane.a +
			   point.y*plane.b +
			   point.z*plane.c +
			   plane.d;
	}

	Point3D intersectionToPlane(Plane &plane, Point3D &neg, Point3D &pos){
		Point3D n(plane.a, plane.b, plane.c);
		Point3D num = n*neg;
		Point3D den = n*(pos-neg);
		double t = (-plane.d - (num.x + num.y + num.z))/
							   (den.x + den.y + den.z);
		return Point3D(neg.x+t*(pos.x-neg.x),
					   neg.y+t*(pos.y-neg.y),
					   neg.z+t*(pos.z-neg.z));;
	}

	TransformationMatrix translate(TransformationMatrix transform, long double x, long double y, long double z){
		TransformationMatrix translate;

		translate.matrix[3][0] = x;
		translate.matrix[3][1] = y;
		translate.matrix[3][2] = z;

		return transform*translate;
	}

	TransformationMatrix rotate(TransformationMatrix transform, long double xAngle, long double yAngle, long double zAngle){
		TransformationMatrix rx, ry, rz;

		rx.matrix[1][1] = cos(xAngle);
		rx.matrix[1][2] = -sin(xAngle);
		rx.matrix[2][1] = sin(xAngle);
		rx.matrix[2][2] = cos(xAngle);

		ry.matrix[0][0] = cos(yAngle);
		ry.matrix[0][2] = sin(yAngle);
		ry.matrix[2][0] = -sin(yAngle);
		ry.matrix[2][2] = cos(yAngle);

		rz.matrix[0][0] = cos(zAngle);
		rz.matrix[0][1] = -sin(zAngle);
		rz.matrix[1][0] = sin(zAngle);
		rz.matrix[1][1] = cos(zAngle);

		return transform*rx*ry*rz;
	}

	TransformationMatrix scale(TransformationMatrix transform, long double xFactor, long double yFactor, long double zFactor){
		TransformationMatrix scale;

		scale.matrix[0][0] = xFactor;
		scale.matrix[1][1] = yFactor;
		scale.matrix[2][2] = zFactor;

		return transform*scale;
	}

}

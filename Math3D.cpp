/*
 * 3DMath.cpp
 *
 *  Created on: Dec 7, 2022
 *      Author: Nathan
 */
#include "Math3D.h"

#include <cmath>

namespace math3d{

	Point3D Point3D::transform(long double* matrix){
		long double arr[3];
		for(int i = 0; i < 4; i++){
			arr[i] = matrix[12+i];
			for(int j = 0; j < 3; j++)
				arr[i] += matrix[i+4*j]*mat[j];
		}
		return Point3D(arr[0], arr[1], arr[2]);
	}

	double distanceToPlane(Point3D point, double a, double b, double c, double d){
		return point.x*a + point.y*b + point.x*c + d;
	}

	long double* getUnitMatrix(){
		return new long double[]{1, 0, 0, 0,
								 0, 1, 0, 0,
								 0, 0, 1, 0,
								 0, 0, 0, 1};
	}

	long double* multiply(long double matrix1[16], long double matrix2[16]){
		long double* out = new long double[16];
		for(int i = 0; i < 4; i++){
			for(int j = 0; j < 4; j++){
				out[j+4*i] = 0;
				for(int l = 0; l < 4; l++){
					out[j+4*i] += matrix1[l+4*i]*matrix2[j+4*l];
				}
			}
		}
		return out;
	}

	long double* translate(long double transform[16], long double x, long double y, long double z){
		long double translate[] = {1, 0, 0, 0,
								   0, 1, 0, 0,
								   0, 0, 1, 0,
								   x, y, z, 1};
		return multiply(transform, translate);
	}

	long double* rotate(long double transform[16], long double xAngle, long double yAngle, long double zAngle){
		long double rx[] = {1, 0, 0, 0,
							0, cos(xAngle), -sin(xAngle), 0,
							0, sin(xAngle), cos(xAngle), 0,
							0, 0, 0, 1};
		long double ry[] = {cos(yAngle), 0, sin(yAngle), 0,
							0, 1, 0, 0,
							-sin(yAngle), 0, cos(yAngle), 0,
							0, 0, 0, 1};
		long double rz[] = {cos(zAngle), -sin(zAngle), 0, 0,
							sin(zAngle), cos(zAngle), 0, 0,
							0, 0, 1, 0,
							0, 0, 0, 1};
		return multiply(transform, multiply(multiply(rx, ry), rz));
	}

	long double* scale(long double transform[16], long double xFactor, long double yFactor, long double zFactor){
		long double scale[] = {xFactor, 0, 0, 0,
							   0, yFactor, 0, 0,
							   0, 0, zFactor, 0,
							   0, 0, 0, 1};
		return multiply(transform, scale);
	}

}

/*
 * 3DMath.h
 *
 *  Created on: Dec 7, 2022
 *      Author: Nathan
 */

#ifndef MATH3D_H_
#define MATH3D_H_

namespace math3d{

	struct Point3D{
		union{
			struct{
				long double x, y, z;
			};
			long double mat[4]{0, 0, 0, 1};
		};

		Point3D() : x(0), y(0), z(0) {};
		Point3D(long double x, long double y, long double z) : x(x), y(y), z(z) {};
		Point3D transform(long double* matrix);
	};

	struct Point2D{
		long double x, y;

		Point2D() : x(0), y(0) {};
		Point2D(long double x, long double y) : x(x), y(y) {};
	};

	struct Plane{
		struct{
			double a, b, c, d;
		};
		double arr[4];
	};

	double distanceToPlane(Point3D point, double a, double b, double c, double d);

	long double* getUnitMatrix();
	long double* multiply(long double matrix1[16], long double matrix2[16]);
	long double* translate(long double transform[16], long double x, long double y, long double z);
	long double* rotate(long double transform[16], long double xAngle, long double yAngle, long double zAngle);
	long double* scale(long double transform[16], long double xFactor, long double yFactor, long double zFactor);

}



#endif /* 3DMATH_H_ */

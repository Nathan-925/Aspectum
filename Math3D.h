/*
 * 3DMath.h
 *
 *  Created on: Dec 7, 2022
 *      Author: Nathan
 */

#ifndef MATH3D_H_
#define MATH3D_H_

namespace math3d{

	struct TransformationMatrix{
		long double matrix[4][4] = {{1, 0, 0, 0},
									{0, 1, 0, 0},
									{0, 0, 1, 0},
									{0, 0, 0, 1}};

		TransformationMatrix operator*(const TransformationMatrix &other);
	};

	struct Point3D{
		union{
			struct{
				long double x, y, z;
			};
			long double mat[4];
		};

		Point3D() : mat{0, 0, 0, 1} {};
		Point3D(long double x, long double y, long double z) : mat{x, y, z, 1} {};
		Point3D transform(const TransformationMatrix &matrix);

		Point3D operator+(const Point3D &other);
		Point3D operator-(const Point3D &other);
		Point3D operator*(const Point3D &other);
		Point3D operator/(const Point3D &other);

		Point3D operator+=(const Point3D &other);
		Point3D operator-=(const Point3D &other);
		Point3D operator*=(const Point3D &other);
		Point3D operator/=(const Point3D &other);

		Point3D operator+(const double &other);
		Point3D operator-(const double &other);
		Point3D operator*(const double &other);
		Point3D operator/(const double &other);

		Point3D operator+=(const double &other);
		Point3D operator-=(const double &other);
		Point3D operator*=(const double &other);
		Point3D operator/=(const double &other);
	};

	struct Vector3D: Point3D{
		Vector3D() : Point3D() { mat[4] = 0; };
		Vector3D(long double x, long double y, long double z) : Point3D(x, y, z) { mat[4] = 0; };
	};

	struct Plane{
		struct{
			double a, b, c, d;
		};
		double arr[4];

		Plane() : a(0), b(0), c(0), d(0) {};
		Plane(double a, double b, double c, double d) : a(a), b(b), c(c), d(d) {};
	};

	double distanceToUnitPlane(Point3D point, Plane plane);
	Point3D intersectionToPlane(Plane &plane, Point3D &neg, Point3D &pos);

	TransformationMatrix translate(TransformationMatrix transform, long double x, long double y, long double z);
	TransformationMatrix rotate(TransformationMatrix transform, long double xAngle, long double yAngle, long double zAngle);
	TransformationMatrix scale(TransformationMatrix transform, long double xFactor, long double yFactor, long double zFactor);

}



#endif /* 3DMATH_H_ */

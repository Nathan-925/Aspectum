/*
 * Interpolation.h
 *
 *  Created on: Dec 7, 2022
 *      Author: Nathan
 */

#ifndef INTERPOLATION_H_
#define INTERPOLATION_H_

namespace intp{
	template <class T>
	T* interpolate(int i0, T d0, int i1, T d1){
		int size = abs(i1-i0);
		T* arr = new T[size+1];
		T slope = (d1-d0)/(i1-i0);
		if(i1 < i0)
			slope *= -1;
		T val = d0;
		for(int i = 0; i < size; i++){
			arr[i] = val;
			val += slope;
		}
		arr[size] = d1;
		return arr;
	}
}

#endif /* INTERPOLATION_H_ */

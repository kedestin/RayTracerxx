// Kevin Destin - Proj 2
#ifndef BOX_H
#define BOX_H

#include <iostream>
#include <utility>
#include <cmath>
#include "ray.h"

using namespace std;


struct Box{
	float min[3], max[3];

	static const int X = 0, Y = 1, Z = 2;

	Box() : Box(0,0,0,0,0,0){}

	Box(float xMax, float yMax, float zMax,
	    float xMin, float yMin, float zMin){
		min[0] = xMin;
		min[1] = yMin;
		min[2] = zMin;

		max[0] = xMax;
		max[1] = yMax;
		max[2] = zMax;
	}
	
	inline float d(int dim) const {
		return max[dim]-min[dim];
	}

	inline float dX() const {
		return d(X);
	}

	inline float dY() const {
		return d(Y);
	}

	inline float dZ() const {
		return d(Z);
	}

	inline void setMax(int dim, float point) {
		max[dim] = point;
	}

	inline void setMin(int dim, float point) {
		min[dim] = point;
	}

	inline float volume() const {
		return dX()*dY()*dZ();
	}

	inline bool isPlanar() const {
		return (volume() == 0);
	}

	inline bool contains(Box b) const {
		for (int i = 0; i < 3; i++){
			if (min[i] > b.min[i] or max[i] < b.min[i])
				return false;
		}

		return true;
	}

	pair<float, float> Intersect(Ray &r) const {
		float tmin, tmax, tymin, tymax, tzmin, tzmax;

 
		tmin  = ( (r.isNeg[0] ? max : min)[0] - r.origin[0]) * r.inv(0);
		tmax  = ( (r.isNeg[0] ? min : max)[0] - r.origin[0]) * r.inv(0);

		tymin = ( (r.isNeg[1] ? max : min)[1] - r.origin[1]) * r.inv(1);
		tymax = ( (r.isNeg[1] ? min : max)[1] - r.origin[1]) * r.inv(1);
		//if ((tmin > tymax) || (tymin > tmax)) 
		//	return pair<float, float>(tmin, tmax); 

		if (tymin > tmin) 
			tmin = tymin; 
		if (tymax < tmax) 
			tmax = tymax; 

		tzmin = ( (r.isNeg[2] ? max : min)[2] - r.origin[2]) * r.inv(2);
		tzmax = ( (r.isNeg[2] ? min : max)[2] - r.origin[2]) * r.inv(2);

		//if ((tmin > tzmax) || (tzmin > tmax)) 
		//	return pair<float, float>(tmin, tmax);

		if (tzmin > tmin) 
			tmin = tzmin;

		if (tzmax < tmax) 
			tmax = tzmax; 

		return pair<float, float>(tmin, tmax); 
	}
};

#endif

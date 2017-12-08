#ifndef RAY_H
#define RAY_H

#include <iostream>
#include "OrderedList.h"
#include <cmath>
#include <climits>

using namespace std;

const float Infinity = ULLONG_MAX;

struct Triangle;

struct Ray{
	Vector origin;
	Vector direction;
	float  t;
	Triangle* hit;
	Triangle* debug;
	bool isNeg[3];

	Ray (float* point, float* direc): origin(point, 3), direction(direc, 3){
		initialize();
	}

	Ray(Point &orig, Vector &direc) {
		origin    = orig;
		direction = direc;

		initialize();
	}

	void initialize(){

		for (int i = 0; i < 3; i++){
			isNeg[i]  = direction[i] < 0; 
		}

		hit = NULL;
		t   = Infinity;		
		debug = NULL;
	}

	inline float inv(int i){
		return (direction[i] == 0 ) ? Infinity : (1/direction[i]);
	} 

	friend ostream &operator<<(ostream &stream, const Ray& ray){
		stream <<"Origin:\n" << ray.origin << endl <<"Direction\n" 
		       <<ray.direction <<endl;

		return stream;
	}

	inline Point intersection(){
		return Point(direction * t  + origin);
	}

};

#endif


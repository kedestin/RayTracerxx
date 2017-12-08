#ifndef POLYOBJECT_H
#define POLYOBJECT_H

#include "ray.h"
#include "Box.h"
#include <iostream>
#include <vector>
#include <string>
#include "rgb.h"
#include "OrderedList.h"
#include "PolyObject.h"
#include <climits>
#include <algorithm>
#include <cassert>

using namespace std;


struct Triangle{
	Point vertices[3];
	Vector normal;
	RGB colors;
	float ks;
	float kd;
	RGB ka;
	bool isLight;

	Triangle(){
		colors.setRGB(127,127,127);
	}

	//Assuming that points are given in Counter Clockwise order
	void setVertices(Point* points){
		for (int i = 0; i < 3; i++){
			vertices[i] = points[i];
		}

		Vector v1(vertices[2] - vertices[1]);
		Vector v2(vertices[0] - vertices[1]);
		normal = v1.cross(v2);
		normal.normalize();


	}

	void setColor(float r, float g, float b){
		colors.setRGB(r,g,b);
		ka = colors * (1 / 10.0);
		kd = 1;
		ks = 1;
	}
 	
	void setLight(bool light){
		isLight = light;
	}

	Box CalcBounds(){
		float xMax, yMax, zMax, xMin, yMin, zMin;
		
		xMax = max(vertices[0][0], max(vertices[1][0], vertices[2][0]));
		yMax = max(vertices[0][1], max(vertices[1][1], vertices[2][1]));
		zMax = max(vertices[0][2], max(vertices[1][2], vertices[2][2])); 

		xMin = min(vertices[0][0], min(vertices[1][0], vertices[2][0]));
		yMin = min(vertices[0][1], min(vertices[1][1], vertices[2][1]));
		zMin = min(vertices[0][2], min(vertices[1][2], vertices[2][2]));

		return Box(xMax, yMax, zMax, xMin, yMin, zMin);
	}

	void Intersect(Ray& tracer){
		float t = -1;
		float d = normal.dot( (Vector) vertices[0]);

		if (normal.dot(tracer.direction) != 0 )
			t = ( d - normal.dot(tracer.origin) )
		            /    normal.dot(tracer.direction);


		if (t < 0)
			return;
		
		Point inter(tracer.origin + tracer.direction * t);

		//Checks if point is within the polygon by using the cross 
		//product and dot product to look at the orientation 
		//of intersection point
		for (int i = 0; i < 3; i++){
			Vector v1( vertices[ (i+1)%3 ] - vertices[i]);
			Vector v2(inter - vertices[i]);
			Vector result = v1.cross(v2);

			if (result.dot(normal) < 0){
				t = -2;
				return ;
			}
		}

		tracer.hit = (t < tracer.t) ? this : tracer.hit;
		tracer.t   = (t < tracer.t) ? t : tracer.t;
		assert(tracer.hit != NULL);
	}
};

struct PolyObject{
	vector<Triangle> mesh;
	Box bbox;
};

#endif
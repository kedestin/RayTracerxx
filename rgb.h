#ifndef RGB_H
#define RGB_H

#include <iostream>
#include "OrderedList.h"
#include <stdexcept>

using namespace std;

struct RGB : public Point{
	RGB() : Point( { 0, 0, 0 }, 3) {}
	RGB(initializer_list<float> newColor) : Point(newColor, 3)  {}
	
	RGB(const OrderedList& toCopy) : Point(toCopy) {}

	
	RGB(float r, float g, float b):Point({r, g, b}, 3){ }

	void setRGB(float r, float g, float b){
		if (r < 0 or g < 0 or b < 0)
			throw runtime_error("Error: RGB must be positive\n");

		data[0] = r;
		data[1] = g;
		data[2] = b;

	}

	float& red(){
		return data[0];
	}

	float& green(){
		return data[1];
	}

	float& blue(){
		return data[2];
	}
};

#endif

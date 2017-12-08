#ifndef CAMERA_H
#define CAMERA_H

#include <iostream>
#include "rgb.h"
#include "ray.h"
#include "OrderedList.h"
#include <stdexcept>

class Camera{
public:
	Camera(int width, int height);
	Camera();
	~Camera();

	RGB&  getPixel(int x, int y);
	int   getHeight();
	int   getWidth();
	float getScreenDistance();
	Ray   getRay(int row, int col);
	void  updatePixel(int row, int col, RGB newColor);
	void  setResolution(int width, int height);
	void  setAngle(float rho, float phi);
	void  setFOV(int inDegrees);

	void  setPixelAspectRatio(float ar);
	void  setPosition(float x, float y, float z);
	void  setScale(float newScale);
	void  operator=(const Camera&);
	RGB  *screen;
	void  translate(float x, float y, float z);
	void  translateX(float x);
	void  translateY(float y);
	void  translateZ(float z);
private:

	float radians(float inDegress);

	bool  pixelInRange(int row, int col);
	void  rotate(float point[], float angle[]);
	void  setScreenDistance();

	int  resolution[2];

	float pixel_AspectRatio, screen_distance, angle[2];
	int FOV;
	Point position;
	float scale;
};

#endif
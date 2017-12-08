
// Kevin Destin - Proj 2
#include <iostream>
#include "Camera.h"
#include "rgb.h"
#include "ray.h"
#include <math.h>
#include "OrderedList.h"

#define PI 3.14159265
using namespace std;

/* For Positions,
 * 	0 - X axis
 *      1 - Y axis
 *      2 - Z axis
 *
 * For angles
 *      0 - angle between X and Y axis
 *      1 - angle between X and Z axis
 *
 * 
 */

Camera::Camera(){
	screen = NULL;
	setPosition(0,0,0);
	setResolution(240, 135);
	setPixelAspectRatio(1);
	setAngle(0,0); 	
	setFOV(90);
	setScale(1);
}

Camera::Camera(int width, int height) {
	screen = NULL;
	setScale(50);
	setResolution(width, height);
	setPixelAspectRatio(1);
	setAngle(0,0);
	setFOV(90);
	setPosition(0, -(float)screen_distance /resolution[1] * scale, 0);

}

Camera::~Camera(){
	delete [] screen;
}

int Camera::getWidth(){
	return resolution[0];
}
int Camera::getHeight(){
	return resolution[1];
}
float Camera::getScreenDistance(){
	return screen_distance;
}

Ray Camera::getRay(int col, int row){
	if (not pixelInRange(col, row)){	
		throw runtime_error("Error: Pixel not in range\n");
	}

	float pixelCenter[3] = {0, 0, 0};
	float p_Width = pixel_AspectRatio, p_Height = 1;
	pixelCenter[2] = ((p_Height * (row + 0.5)) - ((float)(resolution[1]))/2);
	pixelCenter[0] = ((p_Width  * (col + 0.5)) - ((float)(resolution[0]))/2); 	
	pixelCenter[1] = screen_distance; //already scaled

	pixelCenter[2] /= (float)resolution[1] * scale;
	pixelCenter[0] /= (float)resolution[1] * scale;
	pixelCenter[1] /= (float)resolution[1] * scale;



	float *direc = pixelCenter;

	// rotate(direc, angle);

	float origin[3] = {direc[0] + position[0],
	                   direc[1] + position[1],
	                   direc[2] + position[2]};

	Ray toCast = Ray(origin, direc);
	toCast.direction.normalize();

	return toCast;

}
void Camera::updatePixel(int col, int row, RGB newColor){
	getPixel(col, row) = newColor;
}

RGB& Camera::getPixel(int col, int row){
	if (not pixelInRange(col, row)){
		throw runtime_error("Pixel not in range");
	}

	return screen[col + row * getWidth()];
}


float Camera::radians(float inDegrees){
	return inDegrees * PI / 180;
}	

void Camera::setAngle(float rho, float phi){
	angle[0] = rho;
	angle[1] = phi;
}

void Camera::setFOV(int inDegrees){
	if (inDegrees > 10 and inDegrees < 160){ 
		FOV = inDegrees;
		setScreenDistance();
	}
}

void Camera::setScreenDistance(){
	screen_distance  =  ( ((float) resolution[0] * pixel_AspectRatio) / 2 );
	screen_distance /=  tan( radians(FOV/2) ) ; 
}

void Camera::setPixelAspectRatio(float ar){
	pixel_AspectRatio = ar;
}

void Camera::setResolution(int width, int height){
	if (not (width > 0 and height > 0) )
		throw runtime_error("Error: Dimensions must be atleast 1x1\n");

	resolution[0] = width;
	resolution[1] = height;
	
	if (screen != NULL)
		delete screen;

	screen = new RGB[width*height];
}

void Camera::setScale(float newScale){
	if (newScale > 0)
		scale = newScale;
}
void Camera::setPosition(float x, float y, float z){
	position = Point({x, y, z}, 3);
}

bool Camera::pixelInRange(int col, int row){
	if (row < 0 or row >= resolution[1])
		return false;
	if (col < 0 or col >= resolution[0])
		return false;

	return true;
}

void Camera::rotate(float point[], float angle[]){
	point[0] = point[0] * cos(angle[0]) * sin(angle[1]) 
	         - point[1] * sin(angle[0])                 
	         + point[2] * cos(angle[0]);  

	point[1] = point[0] * cos(angle[1]) * sin(angle[0]) 
	         + point[1] * cos(angle[0])                 
	         - point[2] * sin(angle[0]) * sin(angle[1]);  	

	point[2] = point[0] * sin(angle[1])                
	         + point[2] * cos(angle[1]);  
}

void  Camera::translate(float x, float y, float z){
	translateX(x);
	translateY(y);	
	translateZ(z);
}

void  Camera::translateX(float x){
	position[0] = position[0] + x;
}

void  Camera::translateY(float y){
	position[1] = position[1] + y;
}

void  Camera::translateZ(float z){
	position[2] = position[2] + z;

}

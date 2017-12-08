#ifndef SCENE_H
#define SCENE_H

#include <iostream>
#include "Camera.h"
#include "OrderedList.h"
#include "rgb.h"
#include "ray.h"
#include "PolyObject.h"
#include "KDTree.h"

using namespace std;


struct Light {
	Point position;
	RGB intensity;

	Light() : position({ 0, 0, 0 }, 3), intensity(1,1,1) {}

	Light(initializer_list<float> newPos,
	      initializer_list<float> newIntensity)
	    : position(newPos, 3), intensity(newIntensity){}

	Light(Point newPos, RGB newIntensity) : position(newPos),
	                                        intensity(newIntensity) {}

	void setPosition(float x, float y, float z){
		position[0] = x;
		position[1] = y;
		position[2] = z;
	}

	void setPosition(Point &newPosition){
		if (position.dimensions != newPosition.dimensions)
			throw runtime_error("Error: Position must be 3-D\n");

		if (position.numElements != newPosition.numElements)
			throw runtime_error("Error: Position must be 3-D\n");

		position = newPosition;
	}

	void setColor(float r, float g, float b){
		intensity.setRGB(r,g,b);
	}

	void setColor(RGB &newIntensity){
		intensity = newIntensity;
	}
};

class Scene{
private:
	float findIntersection(Ray &tracer, Triangle &triangle);
	RGB   trace(Ray &tracer);
	void  shade(Ray &tracer, RGB& pixel);
	void  BlinnPhong(RGB& color,Ray& tracer, Vector& shadow, Light& light);
	void  diffuse(RGB& pixel, Ray& tracer, Vector& shadow, Light& light);
	void  ambient(RGB& pixel);
	void  buildTree();

	vector<PolyObject> objects;
	vector<Light>      lights;
	KDTree *tree;

public:
	Scene();
	Scene(int, int);
	~Scene();

	void renderScene();
	void addObject(PolyObject);
	void addLight(initializer_list<float> newPos,
	              initializer_list<float> newColor);
	void addLight(Light newLight);
	int 	 numObjects(){
		return objects.size();
	}
	int numLights(){
		return lights.size();
	}
	int getWidth(){
		return camera.getWidth();
	}

	int getHeight(){
		return camera.getHeight();
	}

	
	Camera camera;

 };

#endif
#include <iostream>
#include "Camera.h"
#include "OrderedList.h"
#include "rgb.h"
#include "ray.h"
#include "PolyObject.h"
#include <vector>
#include <climits>
#include "Scene.h"
#include "KDTree.h"
//#include "SAHKDTree.h"
#include <algorithm>

using namespace std;


//#define TERMINAL
#define KDTREE
//#define NORMAL

Scene::Scene(int width, int height) : camera(width, height){
	tree = NULL;
}

Scene::Scene(){
	tree = NULL;
}

Scene::~Scene(){
	if (tree != NULL)
		delete tree;
}


void Scene::renderScene(){
	cout <<"Building tree\n";
	buildTree();
	cout <<"Rendering...\n";
	#ifdef KDTREE
	for (int y = 0; y < camera.getHeight(); y++){
		for (int x = 0; x < camera.getWidth(); x++){
			
			Ray tracer = camera.getRay(x,y);

			if (tree->Intersect(tracer)){
				#ifdef TERMINAL
					cout <<"|";
				#endif				
				shade(tracer, camera.getPixel(x,y)) ;
			}
			else {
				#ifdef TERMINAL
					cout <<" ";
				#endif				
				camera.updatePixel(x, y, RGB(0,0,0)) ;				
			}
			
		}
		#ifdef TERMINAL
			cout <<endl;
		#endif
	}
	#endif
}

void Scene::addObject(PolyObject newObj){
	objects.push_back(newObj);
}

void Scene::addLight(Light newLight){
	lights.push_back(newLight);
}

void Scene::addLight(initializer_list<float> newPos,
                     initializer_list<float> newColor){
	lights.push_back(Light(newPos, newColor));
}

RGB Scene::trace(Ray &tracer){
	float t = Infinity;
	Triangle* closest;
	float intersect;

	for (size_t j = 0; j < objects.size(); j++){
		for (size_t i = 0; i < objects[j].mesh.size(); i++){
			intersect = findIntersection(tracer, objects[j].mesh[i]);
			
			if (0 <= intersect and intersect < t){
				t = intersect;
				closest = &objects[j].mesh[i];
			}
		}
	}

	if (t != Infinity){
		Point inter(tracer.origin + tracer.direction * t);
		#ifdef TERMINAL
			cout <<"|";
		#endif
		tracer.debug = closest;
		tree->Intersect(tracer);
		return RGB(255,255,255);

	}
	else{
		#ifdef TERMINAL
			cout <<" ";
		#endif

		return RGB(0,0,0);
	}
}

float Scene::findIntersection(Ray &tracer, Triangle &triangle){
	Vector normal = triangle.normal;
	float t = -1;
	float d = normal.dot( (Vector) triangle.vertices[0]);

	if (normal.dot(tracer.direction) != 0 )
		t = ( d - normal.dot(tracer.origin) )
	            /    normal.dot(tracer.direction);


	if (t < 0)
		return -1;
	
	Point inter(tracer.origin + tracer.direction * t);

	//Checks if point is within the polygon by using the cross product and 
	//dot product to look at the orientation of intersection point
	for (int i = 0; i < 3; i++){
		Vector v1( triangle.vertices[ (i+1)%3 ] - triangle.vertices[i]);
		Vector v2(inter - triangle.vertices[i]);
		Vector result = v1.cross(v2);

		if (result.dot(normal) < 0)
			return -2;
	}

	return t;	
}

void Scene::shade(Ray &tracer, RGB& pixel){
	pixel.setRGB(0,0,0);

	for ( size_t i = 0; i < lights.size(); i++){
		Point inter = tracer.intersection();
		Vector toLight(lights[i].position - inter);
		Ray shadow(inter, toLight);
		shadow.direction.normalize();

		tree->Intersect(shadow);

		if (Vector(shadow.intersection()).magnitude() >= toLight.magnitude()){
			BlinnPhong(pixel, tracer, shadow.direction, lights[i]);
			diffuse(pixel, tracer, shadow.direction, lights[i]);
		}

		for (int i = 0 ; i < 3; i++){
			if (pixel[i] > 255)
				pixel[i] = 255;
			if (pixel[i] < 0)
				pixel[i] = 0;
		}
	}


}

inline void Scene::ambient(RGB& color){
	(void) color;
}

inline void Scene::BlinnPhong(RGB& color,Ray& tracer, Vector& shadow, Light& light){
	if (shadow.dot(tracer.hit->normal) <= 0)
		return;

	Triangle* hit = tracer.hit;

	Vector h = shadow - tracer.direction;
	h.normalize();

	float BlinnTerm = hit->ks * max(h.dot(hit->normal), 0.0f);


	color = color + (hit->colors * light.intensity * (BlinnTerm));


}

inline void Scene::diffuse(RGB& pixel, Ray& tracer, Vector& shadow,
                           Light& light){
	if (shadow.dot(tracer.hit->normal) <= 0)
		return;

	pixel = pixel + tracer.hit->colors * light.intensity
	                                   * shadow.dot(tracer.hit->normal);
}

void Scene::buildTree(){
	vector<Triangle *> tris;
	int numTris = 0;
	
	for (size_t i = 0; i < objects.size(); i++)
		numTris += objects[i].mesh.size();

	tris.reserve(numTris);

	float xMax, yMax, zMax, xMin, yMin, zMin;
	xMax = yMax = zMax = -Infinity;
	xMin = yMin = zMin =  Infinity;

	for (size_t i = 0; i < objects.size(); i++){
		for(size_t j = 0; j < objects[i].mesh.size(); j++){
			tris.push_back(&(objects[i].mesh[j]));
		}

		xMax = max(xMax, objects[i].bbox.max[0]);
		yMax = max(yMax, objects[i].bbox.max[1]);
		zMax = max(zMax, objects[i].bbox.max[2]); 

		xMin = min(xMin, objects[i].bbox.min[0]);
		yMin = min(xMin, objects[i].bbox.min[1]);
		zMin = min(xMin, objects[i].bbox.min[2]);
	}
	if (tree != NULL)
		delete tree;
	tree = new KDTree(Box(xMax, yMax, zMax, xMin, yMin, zMin), tris);



}


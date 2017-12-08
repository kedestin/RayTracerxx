#include <iostream>
#include "OrderedList.h"
#include "Camera.h"
#include "ray.h"
#include "rgb.h"
using namespace std;


// The camera aperture is at 0 0
// The screen's coordinate system is defined relative to the camera


int main(){
for (int k = 1; k <= 4 ; k++){ 
	cout <<k <<"x" <<k <<endl;
	int square = k;
	int width = square, height = square;
	Camera testCam(width, height );
	for (int j = 0; j < width; j++)
		for (int i = 0; i < height; i++){
			Ray testRay = testCam.getRay(i,j);
			cout <<testRay.direction << endl <<endl;
		}
}
	return 0;
}
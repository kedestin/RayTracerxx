#include <iostream>
#include "Scene.h"
#include "OrderedList.h"
#include "ImageEngine.h"
#include "PolyObject.h"
#include "rgb.h"
#include "Box.h"
#include <thread>
#include <chrono>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <climits>
#include <cassert>

#include "tinyply/source/tinyply.h"


using namespace std;
using namespace tinyply;

PolyObject read_ply_file(const std::string &filename, int scale);

using namespace tinyply;

int main(){
	int multiplier = 2;
	int width = 16 * multiplier, height = 9 * multiplier;
	
	

	Scene testScene(width, height);



	testScene.addObject(read_ply_file("tinyply/assets/sphere.ply", 1));

	testScene.addLight({0,0, 0}, {1,1,1});
	testScene.addLight({0,0, 0}, {1,1,1});
	testScene.addLight({0,0, 0}, {1,1,1});
		testScene.addLight({0,0, 0}, {1,1,1});
	

	testScene.addLight({0,10, 1}, {1,1,1});
	testScene.addLight({100,0, 100}, {1,1,1});

	testScene.camera.setScale(100);

	string input;
	while (cin >> input){
		ImageEngine output;
		output.newImage(width, height);
		float trans = 20;
		switch (input[0]){
			case 'w': testScene.camera.translateY(trans);
				  break;
			case 'a': testScene.camera.translateX(-trans);
				  break;
			case 's': testScene.camera.translateY(-trans);
				  break;
			case 'd': testScene.camera.translateX(trans);
				  break;
			case 'W': testScene.camera.translateZ(trans);
				  break;
			case 'S': testScene.camera.translateZ(-trans);
				  break;
		}
		testScene.renderScene();
		output.copyScreen(testScene.camera.screen);
		output.save("output2.ppm");
		//cout <<"done\n";
	}

	return 0;

}

PolyObject read_ply_file(const std::string & filename, int scale)
{
	// Tinyply can and will throw exceptions at you!
	try
	{
		// Read the file and create a std::istringstream suitable
		// for the lib -- tinyply does not perform any file i/o.
		std::ifstream ss(filename, std::ios::binary);

		// Parse the ASCII header fields
		PlyFile file(ss);

		// Define containers to hold the extracted data. The type must match
		// the property type given in the header. Tinyply will interally allocate the
		// the appropriate amount of memory.
		std::vector<float> verts;
		std::vector<float> norms;
		std::vector<uint8_t> colors;

		std::vector<uint32_t> faces;
		std::vector<float> uvCoords;

		uint32_t vertexCount, normalCount, colorCount, faceCount, faceTexcoordCount, faceColorCount;
		vertexCount = normalCount = colorCount = faceCount = faceTexcoordCount = faceColorCount = 0;

		// The count returns the number of instances of the property group. The vectors
		// above will be resized into a multiple of the property group size as
		// they are "flattened"... i.e. verts = {x, y, z, x, y, z, ...}
		vertexCount = file.request_properties_from_element("vertex", { "x", "y", "z" }, verts);
		normalCount = file.request_properties_from_element("vertex", { "nx", "ny", "nz" }, norms);
		colorCount  = file.request_properties_from_element("vertex", { "red", "green", "blue", "alpha" }, colors);

		// For properties that are list types, it is possibly to specify the expected count (ideal if a
		// consumer of this library knows the layout of their format a-priori). Otherwise, tinyply
		// defers allocation of memory until the first instance of the property has been found
		// as implemented in file.read(ss)
		faceCount = file.request_properties_from_element("face", { "vertex_indices" }, faces, 3);

		file.read(ss);

		PolyObject newObj;
		float xMax, yMax, zMax, xMin, yMin, zMin;
		xMax = yMax = zMax = -Infinity;
		xMin = yMin = zMin =  Infinity;
		for (size_t i = 0; i < faces.size()/3; i++ ){
			uint32_t v1 = faces[i*3], v2=faces[i*3+1], v3=faces[i*3+2];
			Point vertex[3] = {Point({verts[v1*3], verts[v1*3+1], verts[v1*3+2]}, 3) * scale,
			                   Point({verts[v2*3], verts[v2*3+1], verts[v2*3+2]}, 3) * scale,
			                   Point({verts[v3*3], verts[v3*3+1], verts[v3*3+2]}, 3) * scale};

			xMax = max(xMax, max(verts[v1*3]  , max(verts[v2*3]  , verts[v3*3])));
			yMax = max(yMax, max(verts[v1*3+1], max(verts[v2*3+1], verts[v3*3+1])));
			zMax = max(zMax, max(verts[v1*3+2], max(verts[v2*3+2], verts[v3*3+2]))); 

			xMin = min(xMin, min(verts[v1*3]  , min(verts[v2*3]  , verts[v3*3])));
			yMin = min(xMin, min(verts[v1*3+1], min(verts[v2*3+1], verts[v3*3+1])));
			zMin = min(xMin, min(verts[v1*3+2], min(verts[v2*3+2], verts[v3*3+2])));


			Triangle toRender;
			toRender.setVertices(vertex);
		        toRender.setColor(0.5,0.5,0.5);
		
		        newObj.mesh.push_back(toRender);

		}
		newObj.bbox = Box(xMax, yMax, zMax, xMin, yMin, zMin);

		cout <<"done\n";
		return newObj;

	}

	catch (const std::exception & e)
	{
		std::cerr << "Caught exception: " << e.what() << std::endl;
	}
}

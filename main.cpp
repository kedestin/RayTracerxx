

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
#include <string>
#include <sstream>
#include <algorithm>
#include <bits/stdc++.h>

#include "tinyply/source/tinyply.h"

using namespace std;
using namespace tinyply;

PolyObject read_ply_file(const string &filename);
void       getProperties(vector<float>& verts, vector<uint8_t>& colors,
                         vector<uint32_t>& faces, string filename);
void       setVertices(Triangle& toRender, size_t i, vector<uint32_t>& faces,
	               vector<float> verts, Point tri[]);
void       setColors(Triangle& toRender, int i,  vector<uint8_t>& colors);
void       bounds(float mini[], float maxi[], Point tri[]);

void       newScene (istream&, Scene*&);
void       newLight (istream&, Scene*&);
void       newObject(istream&, Scene*&);
void       load     (istream&, Scene*&);
void       debug    (istream&, Scene*&);
void	   render   (istream&, Scene*&);
void       translate(istream&, Scene*&);
void       help     (istream&, Scene*&);
bool       assertScene(Scene*& scene);
void       commandNotFound(string input);
int 	   findCommand(string);
void       run(istream& stream, Scene*& scene);
string     truncate(string&);
void       usageError(string command);
void       Error(string message);
bool 	   isNumA(string isitNum, string type );

const string OUTPUT_FOLDER = "output/";
const string COMMANDS[] = {"newScene", "newLight", "newObject",
                               "load",    "debug",    "render",
                          "translate",    "help"};

const int NUM_COMMANDS = sizeof(COMMANDS)/sizeof(COMMANDS[0]);

void  (* const FUNCTIONS[])(istream&, Scene*& ) = {newScene,
	                                           newLight,
                                                   newObject,
                                                   load,
                                                   debug,
                                                   render,
                                                   translate,
                                           	   help};



int main(){
	Scene* scene = NULL;
	run(cin, scene);

	if (scene != NULL)
		delete scene;

	return 0;
}

void run(istream& stream, Scene*& scene){
	string input;
	if (&stream == &cin)
		cout <<"> ";
	while (stream >> input){
		int command = findCommand(input);

		if (command  == 0)
			newScene(stream, scene);
		else if (command != -1)
	                FUNCTIONS[command](stream, scene);
	        else
	        	commandNotFound(input);

	        if (&stream == &cin)
	        	cout <<"\n> ";
	}
}

int findCommand(string input){
	int i;

	for (i = 0; i < NUM_COMMANDS; i++)
		if (COMMANDS[i] == input)
			break;

	return (i < NUM_COMMANDS ? i : -1);
}

void newScene(istream& stream, Scene*& scene){
	if (scene != NULL)
		delete scene;
	string input;
	int i = 0, resolution[2];
	while (i < 2 and stream >> input){
		if (not (isNumA(input, "int") and 0 < stoi(input))){
			Error("Scene resolution must be positive integers");
			usageError("scene");
			return;
		}

		resolution[i] = stoi(input);
		i++;
	}


	scene = new Scene(resolution[0], resolution[1]);
}

void newLight(istream& stream, Scene*& scene){
	if (not assertScene(scene))
		return;

	stringstream ss;
	string input;
	float vals[6];
	int i = 0;
	while (i < 6 and stream >> input){

		if (not isNumA(input, "float")){
			Error("Error: values must be floating point numbers");
			usageError("light");
			return;
		}

		vals[i] = stof(input);

		if ( vals[i] < 0 and i < 3){
			Error("Error: intensity must be positive");
			usageError("light");
			return;
		}

		i++;
	}

	scene->addLight({vals[3], vals[4], vals[5]},
	               {vals[0], vals[1], vals[2]});	
}

void newObject(istream& stream, Scene*& scene){
	if (not assertScene(scene))
		return;

	string filename;
	stream >> filename;

	if (scene != NULL)
		scene->addObject(read_ply_file(filename));
}

void load(istream& stream, Scene*& scene){
	string filename;
	stream >> filename;

	ifstream instream(filename);

	if (not instream.is_open()){
		cerr <<"Could not open " <<filename <<endl;
		return;
	}
	
	run(instream, scene);
}


void render(istream& stream, Scene*& scene){
	if (not assertScene(scene))
		return;

	ImageEngine output;
	string filename;
	stream >> filename;

	output.newImage(scene->getWidth(), scene->getHeight());
	scene->renderScene();

	output.copyScreen(scene->camera.screen);
	output.save(filename);
}

void translate(istream& stream, Scene*& scene){
	(void) stream;
	(void) scene;
}

void help(istream& stream, Scene*& scene){
	(void) scene;
	string input;
	stream >> input; 
	if (findCommand(input) != -1)
		usageError(input);
	else
		commandNotFound(input);
}

inline string truncate(string& input){
	int maxSize = 12;
	int len = input.size(); 
	return input.substr(0,maxSize) + 
	       (len > maxSize ? (string("...") + input.back()) : "");
}

PolyObject read_ply_file(const string & filename){
	PolyObject newObj;
	float maxi[3], mini[3];
	maxi[0] = maxi[1] = maxi[2] = -Infinity;
	mini[0] = mini[1] = mini[2] =  Infinity;	

	vector<float> verts;
	vector<uint8_t> colors;
	vector<uint32_t> faces;
	getProperties(verts, colors, faces, filename);

	for (size_t i = 0; i < faces.size()/3; i++ ){
		Point tri[3];
		Triangle toRender;
		setVertices(toRender, i, faces, verts, tri);
		bounds(mini,maxi, tri);
		setColors(toRender, i, colors);
	        newObj.mesh.push_back(toRender);
	}
	newObj.bbox = Box(maxi[0], maxi[1], maxi[2], mini[0], mini[1], mini[2]);

	return newObj;
}

void bounds(float mini[], float maxi[], Point tri[]){
	maxi[0] = max(maxi[0], max(tri[0][0], max(tri[1][0], tri[2][0])));
	maxi[1] = max(maxi[1], max(tri[0][1], max(tri[1][1], tri[2][1])));
	maxi[2] = max(maxi[2], max(tri[0][2], max(tri[1][2], tri[2][2])));

	mini[0] = min(mini[0], min(tri[0][0], min(tri[1][0], tri[2][0])));
	mini[1] = min(mini[1], min(tri[0][1], min(tri[1][1], tri[2][1])));
	mini[2] = min(mini[2], min(tri[0][2], min(tri[1][2], tri[2][2])));
}

void getProperties(vector<float>& verts, vector<uint8_t>& colors,
                   vector<uint32_t>& faces, string filename){

	ifstream ss(filename, ios::binary);
	PlyFile file(ss);

	uint32_t vertexCount, colorCount, faceCount;

	vertexCount = colorCount = faceCount = 0;

	initializer_list<string> xyz = {"x", "y", "z"}, f = {"vertex_indices"};
	initializer_list<string> rgb = {"red", "green", "blue"};

	vertexCount = file.request_properties_from_element("vertex", xyz,verts);
	colorCount  = file.request_properties_from_element("face", rgb, colors);
	faceCount   = file.request_properties_from_element("face", f, faces, 3);

	file.read(ss);
}
void setVertices(Triangle& toRender, size_t i, vector<uint32_t>& faces,
		 vector<float> verts, Point tri[]){

	uint32_t v1 = faces[i*3], v2=faces[i*3+1], v3=faces[i*3+2];


	tri[0] =Point({verts[v1*3], verts[v1*3+1], verts[v1*3+2]}, 3);
	tri[1] =Point({verts[v2*3], verts[v2*3+1], verts[v2*3+2]}, 3);
	tri[2] =Point({verts[v3*3], verts[v3*3+1], verts[v3*3+2]}, 3);

	toRender.setVertices(tri);

}

void setColors(Triangle& toRender, int i,  vector<uint8_t>& colors){
	if (colors.size() != 0)
		toRender.colors.setRGB(colors[i*3],colors[i*3+1],colors[i*3+2]);
}

void debug(istream& stream, Scene*& scene ){
	if (not assertScene(scene))
		return;

	(void) stream;
	cout <<"Num Objects: " <<scene->numObjects() <<endl;
	cout <<"Num Lights: "  <<scene->numLights() <<endl;
}

void usageError(string command){
	switch (findCommand(command)){
		case 0: cerr <<"Usage: newScene int int\n";
		        break;
		case 1: cerr <<"Usage: newLight   f f f          f f f\n";
		        cerr <<"                    |              |\n";
		        cerr <<"                intensity      position\n";
		        break;
		case 2: cerr <<"Usage: newObject [path to ply file]\n";
		        break;
		case 3: cerr <<"Usage: load [path to scene file]\n";
		        break;
		case 4: cerr <<"Usage: debug\n";
		        break;
		case 5: cerr <<"Usage: render [path to output file]\n";
		        break;
		case 6: cerr <<"\n";
		        break;
		case 7: cerr <<"Usage: help [command]\n";
		        break;
	}
}

void Error(string message){
	cerr <<"Error: " <<message <<"\n";
}

bool isNumA(string input, string type ){
	bool isInt = (string::npos == input.find_first_not_of("1234567890"));
        bool isFloat = (1 == count(input.begin(), input.end(), '.'));

        if (type == "int")
        	return isInt and not isFloat;
        else if (type == "float")
        	return isInt or isFloat;
        else 
        	return false;
}

bool assertScene(Scene*& scene){
	if (scene == NULL){
		Error("Must make a new scene first");
		return false;
	}

	return true;
}

void commandNotFound(string input){
	Error(string("Command ") + truncate(input)  + " not found");

}
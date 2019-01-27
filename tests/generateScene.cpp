#include <iostream>

using namespace std;

const string COMMANDS[] = {"newScene", "newLight", "newObject",
                               "load", "render", "translate"};


string newScene( unsigned width, unsigned height );
string newLight(double r, double g, double b, int x, int y, int z);
string newObject(string path);
string load(string path);
string render(string path);
string translate();
string preview();
string setPosition(double x, double y, double z);

// TODO: Detect Malformed Primitives (i.e not triangles)
int main() {

        cout << newScene(1920 * 3, 1080 * 3)
             << newObject("./assets/cow.ply")
             << setPosition(-0.375,1.625,3.5)
             << newLight(1.000000,0.000000,0.000000,2147483647,0,0)
             << newLight(1.000000,1.000000,0.000000,2147483647,0,0)
             << newLight(0.000000,1.000000,0.000000,0,2147483647,0)
             << render("cow.ppm")
             << preview();
}

string newScene(unsigned width, unsigned height){
        return  string(__func__) + " " + to_string(width)
                                 + " " + to_string(height) + "\n";
}

string newLight(double r, double g, double b, int x, int y, int z){
        return  string(__func__) + " " + to_string(r) + " "
                                       + to_string(g) + " "
                                       + to_string(b) + " "
                                       + to_string(x) + " "
                                       + to_string(y) + " "
                                       + to_string(z) + "\n";
}

string newObject(string path){
        return  string(__func__) + " " + path + "\n";
}

string load(string path){
        return  string(__func__) + " " + path + "\n";
}

string render(string path){
        return  string(__func__) + " " + path + "\n";
}

string preview(){
        return string(__func__) + "\n";
}
string translate(){
        return  "\n";
}

string setPosition(double x, double y, double z){
        return string(__func__) + " "  + to_string(x) + " "
                                       + to_string(y) + " "
                                       + to_string(z) + "\n";
}
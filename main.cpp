

#include <bits/stdc++.h>
#include <algorithm>
#include <cassert>
#include <chrono>
#include <climits>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include "Box.h"
#include "ImageEngine.h"
#include "OrderedList.h"
#include "PolyObject.h"
#include "Scene.h"
#include "rgb.h"
#include <unistd.h>

void newScene(std::istream&, RayTracerxx::Scene*&);
void newLight(std::istream&, RayTracerxx::Scene*&);
void newObject(std::istream&, RayTracerxx::Scene*&);
void load(std::istream&, RayTracerxx::Scene*&);
void debug(std::istream&, RayTracerxx::Scene*&);
void render(std::istream&, RayTracerxx::Scene*&);
void translate(std::istream&, RayTracerxx::Scene*&);
void help(std::istream&, RayTracerxx::Scene*&);
void preview(std::istream&, RayTracerxx::Scene*&);
void setPosition(std::istream&, RayTracerxx::Scene*&);

void        run(std::istream&, RayTracerxx::Scene*&);
bool        assertScene(RayTracerxx::Scene*& scene);
void        commandNotFound(std::string input);
int         findCommand(std::string);
std::string truncate(std::string&);
void        usageError(std::string command);
void        Error(std::string message);

const std::string COMMANDS[] = {"newScene", "newLight",   "newObject", "load",
                                "debug",    "render",     "translate", "help",
                                "preview",  "setPosition"};

const int NUM_COMMANDS = sizeof(COMMANDS) / sizeof(COMMANDS[0]);

void (*const FUNCTIONS[])(std::istream&, RayTracerxx::Scene*&) = {
    newScene, newLight,  newObject, load,    debug,
    render,   translate, help,      preview, setPosition};

int main() {
        RayTracerxx::Scene* scene = NULL;
        run(std::cin, scene);

        if (scene != NULL)
                delete scene;

        return 0;
}

void run(std::istream& stream, RayTracerxx::Scene*& scene) {
        std::string line, input;
        if (&stream == &std::cin && isatty(fileno(stdin)))
                std::cout << "> ";
        while (stream >> input) {
                int command = findCommand(input);
                if (command != -1)
                        FUNCTIONS[command](stream, scene);
                else
                        commandNotFound(input);

                if (&stream == &std::cin && isatty(fileno(stdin)))
                        std::cout << "\n> ";
        }
}

int findCommand(std::string input) {
        int i;

        for (i = 0; i < NUM_COMMANDS; i++)
                if (COMMANDS[i] == input)
                        break;

        return (i < NUM_COMMANDS ? i : -1);
}

void newScene(std::istream& stream, RayTracerxx::Scene*& scene) {
        if (scene != NULL)
                delete scene;
        std::string input;
        int         resolution[2];
        for (int i = 0; i < 2 and stream >> input; i++) {
                try {
                        resolution[i] = stoi(input);
                        if (resolution[i] < 0)
                                throw std::logic_error("");
                } catch (const std::logic_error& e) {
                        Error(
                            "Scene resolution must be valid "
                            "positive integers");
                        usageError("scene");
                        return;
                }
        }

        scene = new RayTracerxx::Scene(resolution[0], resolution[1]);
}

void newLight(std::istream& stream, RayTracerxx::Scene*& scene) {
        if (not assertScene(scene))
                return;

        std::string           input;
        RayTracerxx::Number_t vals[6];

        for (int i = 0; i < 6 and stream >> input; i++) {
                try {
                        vals[i] = stof(input);
                } catch (std::logic_error& e) {
                        Error("Error: values must be floating point numbers");
                        usageError("light");
                        return;
                }

                if (vals[i] < 0 and i < 3) {
                        Error("Error: intensity must be positive");
                        usageError("light");
                        return;
                }
        }

        scene->addLight({vals[3], vals[4], vals[5]},
                        {vals[0], vals[1], vals[2]});
}

void newObject(std::istream& stream, RayTracerxx::Scene*& scene) {
        if (not assertScene(scene))
                return;

        std::string filename;
        stream >> filename;

        if (scene != NULL)
                scene->addObject(RayTracerxx::PolyObject(filename));
}

void load(std::istream& stream, RayTracerxx::Scene*& scene) {
        std::string filename;
        stream >> filename;

        std::ifstream instream(filename);

        if (not instream.is_open()) {
                std::cerr << "Could not open " << filename << "\n";
                return;
        }

        run(instream, scene);
}

void debug(std::istream& stream, RayTracerxx::Scene*& scene) {
        if (not assertScene(scene))
                return;

        (void)stream;
        std::cout << "Num Objects: " << scene->numObjects() << "\n";
        std::cout << "Num Lights: " << scene->numLights() << "\n";
}

void render(std::istream& stream, RayTracerxx::Scene*& scene) {
        if (not assertScene(scene))
                return;
        RayTracerxx::ImageEngine output;
        std::string              filename;
        stream >> filename;

        output.newImage(scene->getWidth(), scene->getHeight());
        scene->renderScene();
        output.copyScreen(scene->camera.screen);
        output.save(filename);
}

void preview(std::istream& stream, RayTracerxx::Scene*& scene) {
        if (not assertScene(scene))
                return;
        RayTracerxx::Number_t prevH = scene->camera.getHeight();
        RayTracerxx::Number_t prevW = scene->camera.getWidth();
        RayTracerxx::Number_t pixel_aspectratio =
            scene->camera.getPixelRatio();
        char c;
        bool quit = false;

        scene->camera.setPixelAspectRatio(0.5);
        if (prevW > 80)
                scene->camera.setResolution(80, prevH * (80 / prevW));
        scene->preview();
        while (stream >> c && c != 'x') {  // x for exit
                RayTracerxx::Number_t step = 4;
                switch (c) {
                        case 'w':
                        case 'W': scene->camera.translateY(-step); break;
                        case 'q':
                        case 'Q': scene->camera.translateZ(-step); break;
                        case 'd':
                        case 'D': scene->camera.translateX(-step); break;
                        case 'e':
                        case 'E': scene->camera.translateZ(+step); break;
                        case 's':
                        case 'S': scene->camera.translateY(+step); break;
                        case 'a':
                        case 'A': scene->camera.translateX(+step); break;
                        case 'x':
                        case 'X': quit = true; break;
                        default:;
                }
                scene->preview();
        }
        scene->camera.setResolution(prevW, prevH);
        scene->camera.setPixelAspectRatio(pixel_aspectratio);
}
void translate(std::istream& stream, RayTracerxx::Scene*& scene) {
        if (not assertScene(scene))
                return;

        std::string           input;
        RayTracerxx::Number_t position[3];
        for (int i = 0; i < 3 and stream >> input; i++) {
                try {
                        position[i] = stof(input);
                } catch (const std::logic_error& e) {
                        Error(
                            "Scene position must be valid "
                            "floating point numbers");
                        usageError("scene");
                        return;
                }
        }
        scene->camera.translate(position[0], position[1], position[2]);
}

void help(std::istream& stream, RayTracerxx::Scene*& scene) {
        (void)scene;
        std::string input;
        stream >> input;
        if (findCommand(input) != -1)
                usageError(input);
        else
                commandNotFound(input);
}

void setPosition(std::istream& stream, RayTracerxx::Scene*& scene) {
        if (not assertScene(scene))
                return;

        std::string           input;
        RayTracerxx::Number_t position[3];
        for (int i = 0; i < 3 and stream >> input; i++) {
                try {
                        position[i] = stof(input);
                } catch (const std::logic_error& e) {
                        Error(
                            "Scene position must be valid "
                            "floating point numbers");
                        usageError("scene");
                        return;
                }
        }
        scene->camera.setPosition(position[0], position[1], position[2]);
}

std::string truncate(std::string& input) {
        int maxSize = 15;
        int len     = input.size();
        return input.substr(0, maxSize - strlen("...")) +
               (len > maxSize ? ("..." + input.substr(len - 1, 1)) : "");
}

void usageError(std::string command) {
        switch (findCommand(command)) {
                case 0: std::cerr << "Usage: newScene int int\n"; break;
                case 1:
                        std::cerr << "Usage: newLight  f f f          f f f\n";
                        std::cerr << "                   |              |\n";
                        std::cerr
                            << "               intensity      position\n";
                        break;
                case 2:
                        std::cerr << "Usage: newObject [path to ply file]\n";
                        break;
                case 3:
                        std::cerr << "Usage: load [path to scene file]\n";
                        break;
                case 4: std::cerr << "Usage: debug\n"; break;
                case 5:
                        std::cerr << "Usage: render [path to output file]\n";
                        break;
                case 6: std::cerr << "Usage: setPosition  f f f\n"; break;
                case 7: std::cerr << "Usage: help [command]\n"; break;
                case 8: std::cerr << "Usage: preview\n"; break;
                case 9: std::cerr << "Usage: setPosition  f f f\n"; break;
                default: break;
        }
}

void Error(std::string message) { std::cerr << "Error: " << message << "\n"; }

bool assertScene(RayTracerxx::Scene*& scene) {
        if (scene == NULL) {
                Error("Must make a new scene first");
                return false;
        }

        return true;
}

void commandNotFound(std::string input) {
        Error(std::string("Command ") + truncate(input) + " not found");
}
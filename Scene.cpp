#include "Scene.h"
#include <climits>
#include <iostream>
#include <vector>
#include "Camera.h"
#include "OrderedList.h"
#include "PolyObject.h"
#include "ray.h"
#include "rgb.h"
#define TESTING
#ifndef TESTING
#include "KDTree.h"
#else
#include "KDTree2.h"
#endif
#include <algorithm>
#include <chrono>

namespace RayTracerxx {

// #define TERMINAL
#define KDTREE
//#define NORMAL

Scene::Scene(int width, int height) : camera(width, height) {
        tree            = NULL;
        hasBeenModified = false;
}

Scene::Scene() {
        tree            = NULL;
        hasBeenModified = false;
}

Scene::~Scene() {
        if (tree != NULL)
                delete tree;
}

void Scene::preview() {
        bool preview = true;
        renderScene(preview);
        std::cout << camera << "\n";
}

/**
 * @brief      Rebuilds the tree if it had been has been modified
 *             Shades each pixel of the camera screen using rays projected
 *             from the camera
 */
void Scene::renderScene(bool preview) {
        using namespace std::chrono;

        if (hasBeenModified) {
                std::cout << "Building tree\n";
                auto start = high_resolution_clock::now();
                buildTree();
                auto end        = high_resolution_clock::now();
                hasBeenModified = false;
                std::cout << "Build time: "
                          << duration_cast<seconds>(end - start).count()
                          << " seconds\n";
        }

        std::cout << "Rendering...\n";
        auto t1 = high_resolution_clock::now();
        for (int y = 0; y < camera.getHeight(); y++) {
                for (int x = 0; x < camera.getWidth(); x++) {
                        Ray tracer = camera.getRay(x, y);

                        if (tree != NULL && tree->Intersect(tracer)) {
                                if (preview)
                                        std::cout << "|";
                                else
                                        shade(tracer, camera.getPixel(x, y));

                        } else {
                                if (preview)
                                        std::cout << ".";
                                else
                                        camera.updatePixel(x, y, RGB(0, 0, 0));
                        }
                }
                if (preview)
                        std::cout << "\n";
        }
        auto t2 = high_resolution_clock::now();

        std::cout << "Elapsed time: "
                  << duration_cast<milliseconds>(t2 - t1).count()
                  << " milliseconds\n";
}

void Scene::addObject(PolyObject newObj) {
        objects.push_back(newObj);
        hasBeenModified = true;
}

void Scene::addLight(Light newLight) {
        lights.push_back(newLight);
        hasBeenModified = true;
}

void Scene::addLight(std::initializer_list<Number_t> newPos,
                     std::initializer_list<Number_t> newColor) {
        lights.push_back(Light(newPos, newColor));
        hasBeenModified = true;
}

/**
 * @brief      Iterates through all the lights in the scene
 *             If the tracer ray has an unobstructed view of a light shader
 *             models are applied
 *
 * @param      tracer  The tracer
 * @param      pixel   The pixel
 */
void Scene::shade(Ray& tracer, RGB& pixel) {
        pixel.setRGB(0, 0, 0);

        for (size_t i = 0; i < lights.size(); i++) {
                // Move the intersection point away from triangle
                Point<3> inter =
                    tracer.intersection() +
                    (tracer.hit->normal * tracer.intersectionBias);
                Vector<3> toLight(lights[i].position - inter);
                Ray       shadow(inter, toLight);
                shadow.direction.normalize();

                tree->Intersect(shadow);
                if (Vector<3>(shadow.intersection() - shadow.origin).norm() >=
                    toLight.norm()) {
                        //     std::cerr<<"/";
                        BlinnPhong(pixel, tracer, shadow.direction, lights[i]);
                        diffuse(pixel, tracer, shadow.direction, lights[i]);
                }

                for (unsigned j = 0; j < 3; j++) {
                        if (pixel[j] > 255)
                                pixel[j] = 255;
                        if (pixel[j] < 0)
                                pixel[j] = 0;
                }
        }
}

/**
 * @brief      Updates pixels with contribution of the BlingPhong model
 *
 * @details    https://paroj.github.io/gltut/Illumination/
 *             Tut11%20BlinnPhong%20Model.html
 *             https://en.wikipedia.org/wiki/Blinn%E2%80%93Phong_shading_model
 */
void Scene::BlinnPhong(RGB& pixel, Ray& tracer, Vector<3>& shadow,
                       Light& light) {
        if (shadow.dot(tracer.hit->normal) <= 0)
                return;

        Triangle* hit = tracer.hit;

        Vector<3> h = shadow - tracer.direction;
        h.normalize();

        Number_t BlinnTerm =
            hit->ks * std::max(h.dot(hit->normal), (Number_t)0.0);

        pixel = pixel + (hit->color * light.intensity * BlinnTerm);
}

/**
 * @brief      Computes the lambertian contribution to pixel color
 *
 * @details    Lambert =   Intensity * Material_Reflectance
 * Unit_Normal dot Light_Vector
 *
 *             https://www.wikilectures.eu/w/Lambert%27s_law
 */
void Scene::diffuse(RGB& pixel, Ray& tracer, Vector<3>& shadow, Light& light) {
        if (shadow.dot(tracer.hit->normal) <= 0)
                return;

        pixel = pixel + tracer.hit->color * light.intensity *
                            shadow.dot(tracer.hit->normal);
}

/**
 * @brief      Retrieves all the triangles from all the objects
 *             Determines the bounding box enclosing all of them
 *             Builds KDTree
 */
void Scene::buildTree() {
        std::vector<Triangle*> tris;
        int                    numTris = 0;

        for (size_t i = 0; i < objects.size(); i++)
                numTris += objects[i].mesh.size();

        tris.reserve(numTris);

        Number_t xMax, yMax, zMax, xMin, yMin, zMin;
        xMax = yMax = zMax = -Ray::Infinity;
        xMin = yMin = zMin = Ray::Infinity;

        for (size_t i = 0; i < objects.size(); i++) {
                for (size_t j = 0; j < objects[i].mesh.size(); j++) {
                        tris.push_back(&(objects[i].mesh[j]));
                }

                xMax = std::max(xMax, objects[i].bbox.hi[0]);
                yMax = std::max(yMax, objects[i].bbox.hi[1]);
                zMax = std::max(zMax, objects[i].bbox.hi[2]);

                xMin = std::min(xMin, objects[i].bbox.low[0]);
                yMin = std::min(xMin, objects[i].bbox.low[1]);
                zMin = std::min(xMin, objects[i].bbox.low[2]);
        }
        if (tree != NULL)
                delete tree;
        tree = new KDTree(Box(xMax, yMax, zMax, xMin, yMin, zMin), tris);
}

}  // namespace RayTracerxx
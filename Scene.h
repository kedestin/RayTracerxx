#ifndef SCENE_H
#define SCENE_H

#include <iostream>
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

namespace RayTracerxx {

class Scene {
private:
        /**
         * @brief      Representation of a point light
         */
        struct Light {
                Point<3> position;
                RGB      intensity;

                Light() : position(), intensity(1, 1, 1) {}

                Light(std::initializer_list<Number_t> newPos,
                      std::initializer_list<Number_t> newIntensity)
                    : position(begin(newPos)), intensity(newIntensity) {}

                Light(Point<3> newPos, RGB newIntensity)
                    : position(newPos), intensity(newIntensity) {}

                void setPosition(Number_t x, Number_t y, Number_t z) {
                        position[0] = x;
                        position[1] = y;
                        position[2] = z;
                }

                void setPosition(Point<3>& newPosition) {
                        position = newPosition;
                }

                void setColor(Number_t r, Number_t g, Number_t b) {
                        intensity.setRGB(r, g, b);
                }

                void setColor(RGB& newIntensity) { intensity = newIntensity; }
        };

        /**
         * @brief      Computes the correct color of a pixel in the screen
         *
         * @param      tracer  The tracer
         * @param      pixel   The pixel
         */
        void shade(Ray& tracer, RGB& pixel);

        /**
         * @brief      Computes the contribution of the BlinnPhong shader
         *             model
         *
         * @param      p     pixel
         * @param      t     tracer ray
         * @param      s     shadow vector
         * @param      l     point light
         */
        void BlinnPhong(RGB& p, Ray& t, Vector<3>& s, Light& l);

        /**
         * @brief      Computes the contribution of the diffuse shader model
         *
         * @param      p     pixel
         * @param      t     tracer ray
         * @param      s     shadow vector
         * @param      l     point light
         */
        void diffuse(RGB& p, Ray& t, Vector<3>& s, Light& l);

        /**
         * @brief      Computes the contribution of the ambient shader model
         *
         * @param      pixel  The pixel
         */
        void ambient(RGB& pixel);

        /**
         * @brief      Builds the KD-Tree
         */
        void buildTree();

        std::vector<PolyObject> objects;
        std::vector<Light>      lights;
        KDTree*                 tree;
        bool                    hasBeenModified;

public:
        Scene();
        ~Scene();

        /**
         * @brief      Constructs a scene, specifying the camera dimensions
         *
         * @param[in]  width   The width
         * @param[in]  height  The height
         */
        Scene(int width, int height);

        /**
         * @brief      Takes a "picture" of scene with Camera
         *
         * @details    The value of each pixel is computed
         *
         * @param[in]  preview  Whether a preview is being rendered
         */
        void renderScene(bool preview = false);

        /**
         * @brief      Generates a preview of scene through standard out
         */
        void preview();

        /**
         * @brief      Adds an object to Scene.
         *
         * @param[in]  <unnamed>  PolyObject to add
         */
        void addObject(PolyObject);

        /**
         * @brief      Adds a light.
         *
         * @param[in]  newPos    The new position
         * @param[in]  newColor  The new color
         */
        void addLight(std::initializer_list<Number_t> newPos,
                      std::initializer_list<Number_t> newColor);

        /**
         * @brief      Adds a light.
         *
         * @param[in]  newLight  The new light
         */
        void addLight(Light newLight);

        /**
         * @brief      { function_description }
         *
         * @details    { detailed_item_description }
         *
         * @return     { description_of_the_return_value }
         */
        unsigned numObjects() { return objects.size(); }

        /**
         * @brief      Gets the number of lights
         *
         * @return     { description_of_the_return_value }
         */
        unsigned numLights() { return lights.size(); }

        /**
         * @brief      Gets the width
         *
         * @return     The width.
         */
        unsigned getWidth() { return camera.getWidth(); }

        /**
         * @brief      Gets the height.
         *
         * @return     The height.
         */
        unsigned getHeight() { return camera.getHeight(); }

        Camera camera;
};
}  // namespace RayTracerxx
#endif
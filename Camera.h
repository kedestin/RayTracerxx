#ifndef CAMERA_H
#define CAMERA_H

#include <iostream>
#include <stdexcept>
#include "OrderedList.h"
#include "ray.h"
#include "rgb.h"

namespace RayTracerxx {

/**
 * @brief      Class for camera.
 *
 * @details    The camera is modeled as an emitter which projects rays through
 *             a screen. An seperate model determines what the ray interacts
 *             with an updates the camera accordingly.
 */
class Camera {
public:
        enum COORDINATES { X = 0, Y = 1, Z = 2 };
        enum DIMENSIONS { W = 0, H = 1 };
        Camera(int width, int height);
        Camera();
        ~Camera();

        /**
         * @brief      Gets the pixel.
         *
         * @details    { detailed_item_description }
         *
         * @param[in]  col   The col
         * @param[in]  row   The row
         *
         * @return     The pixel.
         */
        RGB& getPixel(int col, int row);

        /**
         * @brief      Gets the ray passing throw the requested pixel.
         *
         * @param[in]  row   The row
         * @param[in]  col   The col
         *
         * @return     The ray.
         */
        Ray getRay(unsigned col, unsigned row);

        /**
         * @brief      Returns the vertical component of the screen's
         *             dimensions
         *
         * @return     The height.
         */
        int getHeight();

        /**
         * @brief      Gets the horizontal component of the screen's
         *             dimensions
         *
         * @return     The width.
         */
        int getWidth();

        /**
         * @brief      Gets the distance of the emitter to the screen
         *
         * @return     The screen distance.
         */
        Number_t getScreenDistance();

        /**
         * @brief      Gets the pixel ratio.
         *
         * @details    Ratio of pixel's width to pixel's height
         *             i.e If the pixel is twice as tall as it is wide, the
         *                 aspect ratio is 0.5
         *
         * @return     The pixel ratio.
         */
        Number_t getPixelRatio();

        /**
         * @brief      Updates the screen pixel's RGB value
         *
         * @param[in]  row       The row
         * @param[in]  col       The col
         * @param[in]  newColor  The new color
         */
        void updatePixel(unsigned col, unsigned row, RGB newColor);

        /**
         * @brief      Sets the resolution of screen
         *
         * @param[in]  width   The width
         * @param[in]  height  The height
         */
        void setResolution(int width, int height);

        /**
         * @brief      Sets the horizontal FOV to provided argument
         *             Arguments are forced within range [20; 160]
         *
         * @param[in]  inDegrees  In degrees
         */
        void setFOV(int inDegrees);

        /**
         * @brief      Sets the pixel aspect ratio.
         *
         * @details    Ratio of pixel's width to pixel's height
         *             i.e If the pixel is twice as tall as it is wide, the
         *                 aspect ratio is 0.5
         *
         * @param[in]  ar    The aspect ratio
         */
        void setPixelAspectRatio(Number_t ar);

        /**
         * @brief      Sets the position of the camera
         *
         * @param[in]  x     X coordinate
         * @param[in]  y     Y coordinate
         * @param[in]  z     Z coordinate
         */
        void setPosition(Number_t x, Number_t y, Number_t z);

        /**
         * @brief      Sets the scaling factor of the camera
         *
         *
         * @param[in]  newScale  The new scale
         */
        void setScale(Number_t newScale);

        /**
         * @brief      Translates the camera relative to its current position
         *
         *
         * @param[in]  x     X offset
         * @param[in]  y     Y offset
         * @param[in]  z     Z offset
         */
        void translate(Number_t x, Number_t y, Number_t z);
        void translateX(Number_t x);
        void translateY(Number_t y);
        void translateZ(Number_t z);

        friend std::ostream& operator<<(std::ostream& out, const Camera& c) {
                return out << "Position:" << c.position << '\n'
                           << "Horizontal FOV (degrees): " << c.FOV << '\n'
                           << "Scaling Factor: " << c.scale << '\n';
        }
        RGB* screen;

private:
        /**
         * @brief      Converts angle in degrees to radians
         *
         * @param[in]  inDegress  In degress
         *
         * @return     Angle in radians
         */
        Number_t radians(Number_t inDegress);

        /**
         * @brief      Performs boundchecking on the indices
         *
         * @param[in]  col   The col
         * @param[in]  row   The row
         *
         * @return     True if indices are in range,
         *             False otherwise
         */
        bool pixelInRange(int col, int row);
        // void rotate(Number_t point[], Number_t angle[]);

        /**
         * @brief      Sets the distance of the emitter to the screen. Depends
         *             on the FOV
         */
        void     setScreenDistance();

        Point<3> position;
        int      resolution[2];

        Number_t pixel_AspectRatio, screen_distance;
        int      FOV;
        Number_t scale;
};
}  // namespace RayTracerxx
#endif
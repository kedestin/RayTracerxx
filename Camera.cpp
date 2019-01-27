
// Kevin Destin - Proj 2
#include "Camera.h"
#include <math.h>
#include <iostream>
#include "OrderedList.h"
#include "ray.h"
#include "rgb.h"

#define PI 3.14159265

namespace RayTracerxx {

Camera::Camera() {
        screen = NULL;
        setPosition(0, 0, 0);
        setResolution(240, 135);
        setPixelAspectRatio(1);
        setScale(1);
        setFOV(90);
}

Camera::Camera(int width, int height) : Camera() {
        setResolution(width, height);
        setScale(.5);
        // setPosition(0, -(Number_t)screen_distance /resolution[H] * scale,0);
        // setPosition(0, -30, 0);     //Sphere
        // setPosition(-50,-566, 170); //Sphere
        // setPosition(-50, -196, 0);  //Sphere
        // setPosition(-32, -336, 20); //Sphere
        // setPosition(-40, -56, -10); //Choppper
        // setPosition(-30,-316, 140); //Dolphins
        setPosition(8, -8, -4);  // F16
        // setPosition(-2, -4, 0);     //Icosahedron
        // setPosition(0, 0, 0);
        // setPosition(0, 0.125, 0);  // Dragon
        // setPosition(-4, -12, -316); //1e7 statue
}

Camera::~Camera() { delete[] screen; }

int      Camera::getWidth() { return resolution[W]; }
int      Camera::getHeight() { return resolution[H]; }
Number_t Camera::getScreenDistance() { return screen_distance; }
Number_t Camera::getPixelRatio() { return pixel_AspectRatio; }

/**
 * @brief      Gets the ray.
 *
 * @details
 *             Assumptions:
 *
 *             Position refers to the centermost point of the screen
 *             The camera is aligned on the perpendicular extending from the
 *             center
 *             Ray originates at the camera, passing through the center of the
 *             requested pixel
 *
 *             Procedure:
 *
 *             Assumes that the screen is centered at the origin.
 *             Computes coordinates of center pixel.
 *             Performs any necessary transformation (rotation, translation)
 *             afterwards
 *
 * @param[in]  col   The col
 * @param[in]  row   The row
 *
 * @return     The ray.
 */
Ray Camera::getRay(unsigned col, unsigned row) {
        if (not pixelInRange(col, row))
                throw std::runtime_error("Error: Pixel not in range\n");
        setScreenDistance();
        Number_t pixelCenter[3] = {0, 0, 0}, origin[3];
        Number_t p_Width = pixel_AspectRatio;  // ratio of pixel width to
                                               // height

        pixelCenter[Y] =
            -((row + 0.5) - (static_cast<Number_t>(resolution[H]) / 2));
        pixelCenter[X] =
            (col + 0.5) - (static_cast<Number_t>(resolution[W]) / 2);
        pixelCenter[Z] = 0;
        pixelCenter[X] *= p_Width;  // Scaling X seems to lessen distortion on
                                    // terminal
        // pixelCenter[Y] /= p_Width;  // Or should I scale "Y"?

        // Origin is axis aligned
        origin[X] = 0;
        origin[Y] = 0;
        origin[Z] = screen_distance;

        for (int i = 0; i < 3; i++) {
                // Normalize all values so that the screen has an area of one
                // (arbitrary choice)
                pixelCenter[i] /= sqrt(resolution[H] * resolution[W]);
                origin[i] /= sqrt(resolution[H] * resolution[W]);

                // Scale all values
                pixelCenter[i] *= scale;
                origin[i] *= scale;

                // translate all values
                pixelCenter[i] += position[i];
                origin[i] += position[i];
        }

        Number_t* direc = pixelCenter;

        // Compute the ray
        for (int i = 0; i < 3; i++)
                direc[i] -= origin[i];

        Ray toCast(origin, direc);
        toCast.direction.normalize();
        // std::cerr << toCast <<std::endl;
        return toCast;
}

void Camera::updatePixel(unsigned col, unsigned row, RGB newColor) {
        getPixel(col, row) = newColor;
}

/**
 * @brief      Gets the pixel from the screen
 *
 * @param[in]  col   The col
 * @param[in]  row   The row
 *
 * @return     RGB& Reference to the pixel
 */
RGB& Camera::getPixel(int col, int row) {
        if (not pixelInRange(col, row)) {
                throw std::runtime_error("Pixel not in range");
        }

        return screen[col + row * getWidth()];
}

Number_t Camera::radians(Number_t inDegrees) { return inDegrees * (PI / 180); }

void Camera::setFOV(int inDegrees) {
        if (inDegrees < 10)
                inDegrees = 10;
        if (inDegrees > 160)
                inDegrees = 160;

        FOV = inDegrees;
        setScreenDistance();
}

/**
 * @details    Solves equation for screen distance to guarantee FOV is as
 *             specified
 */
void Camera::setScreenDistance() {
        screen_distance = (((Number_t)resolution[W] * pixel_AspectRatio) / 2);
        screen_distance /= tan(radians(FOV / 2));
}

void Camera::setPixelAspectRatio(Number_t ar) { pixel_AspectRatio = ar; }

/**
 * @brief      Sets the resolution.
 *
 * @details    Reallocates the screen if necessary
 *
 * @param[in]  width   The width
 * @param[in]  height  The height
 */
void Camera::setResolution(int width, int height) {
        if (not(width > 0 and height > 0))
                throw std::runtime_error(
                    "Error: Dimensions must "
                    "be atleast 1x1\n");

        resolution[W] = width;
        resolution[H] = height;

        if (screen != NULL)
                delete[] screen;

        screen = new RGB[width * height];
}

void Camera::setScale(Number_t newScale) {
        if (newScale > 0)
                scale = newScale;
}

void Camera::setPosition(Number_t x, Number_t y, Number_t z) {
        Number_t toAdd[3] = {x, y, z};
        position          = Point<3>(toAdd);
}

bool Camera::pixelInRange(int col, int row) {
        if (row < 0 or row >= resolution[H])
                return false;
        if (col < 0 or col >= resolution[W])
                return false;

        return true;
}

// void Camera::rotate(Number_t point[], Number_t a[]) {
//         point[X] = point[X] * cos(a[0]) * sin(a[1]) - point[Y] * sin(a[0]) +
//                    point[Z] * cos(a[0]);

//         point[Y] = point[X] * cos(a[1]) * sin(a[0]) + point[Y] * cos(a[0]) -
//                    point[Z] * sin(a[0]) * sin(a[1]);

//         point[Z] = point[X] * sin(a[1]) + point[Z] * cos(a[1]);
// }

void Camera::translate(Number_t x, Number_t y, Number_t z) {
        translateX(x);
        translateY(y);
        translateZ(z);
}

void Camera::translateX(Number_t x) { position[X] += x; }

void Camera::translateY(Number_t y) { position[Y] += y; }

void Camera::translateZ(Number_t z) { position[Z] += z; }

}  // namespace RayTracerxx
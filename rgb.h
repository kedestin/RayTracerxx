#ifndef RGB_H
#define RGB_H

#include <iostream>
#include <stdexcept>
#include "OrderedList.h"

namespace RayTracerxx {

/**
 * @brief      Pixel (tuple of red, green, and blue components) implementation
 */
struct RGB : public Point<3> {
        RGB() : Point<3>() {}
        RGB(std::initializer_list<Number_t> newColor)
            : Point<3>(begin(newColor)) {}

        RGB(const OrderedList<3>& toCopy) : Point<3>(toCopy) {}

        RGB(Number_t r, Number_t g, Number_t b)
            : Point<3>(std::begin({r, g, b})) {}

        void setRGB(Number_t r, Number_t g, Number_t b) {
                if (r < 0 or g < 0 or b < 0)
                        throw std::runtime_error(
                            "Error: RGB must "
                            "be positive\n");

                data[0] = r;
                data[1] = g;
                data[2] = b;
        }

        Number_t& red() { return data[0]; }

        Number_t& green() { return data[1]; }

        Number_t& blue() { return data[2]; }
};
}  // namespace RayTracerxx
#endif

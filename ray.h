#ifndef RAY_H
#define RAY_H

#include <climits>
#include <cmath>
#include <iostream>
#include <limits>
#include "OrderedList.h"

namespace RayTracerxx {
struct Triangle;

struct Ray {
public:
        // Avoid odr-used, don't take address without explicitly declaring it
        // Linker errors if Infinity isn't used as a value (vs. variable)
        static constexpr Number_t Infinity =
            std::numeric_limits<Number_t>::max();
        Point<3>  origin;
        Vector<3> direction;
        Number_t  t;
        Triangle *hit;
        bool      isNeg[3];
        Number_t  intersectionBias = 1e-6;

        Ray(const Number_t point[3], const Number_t direc[3])
            : origin(point), direction(direc) {
                initialize();
        }

        Ray(const Point<3> &orig, const Vector<3> &direc)
            : origin(orig), direction(direc) {
                initialize();
        }

        /**
         * @brief      Inverse of Ray direction in dimension i
         *
         * @param[in]  k     Dimension
         *
         * @return     The inverse of the direction
         *             If the component of direction is 0, returns
         *             Ray::Infinity
         */
        Number_t inv(int k) {
                return (direction[k] == 0) ? Infinity : (1 / direction[k]);
        }

        friend std::ostream &operator<<(std::ostream &stream, const Ray &ray) {
                stream << "Origin:\n"
                       << ray.origin << "\n"
                       << "Direction\n"
                       << ray.direction << "\n";

                return stream;
        }

        /**
         * @brief      Computes the intersection point with the triangle it
         *             hit
         *
         * @return     Intersection point
         */
        Point<3> intersection() { return direction * t + origin; }

private:
        void initialize() {
                for (int i = 0; i < 3; i++) {
                        isNeg[i] = direction[i] < 0;
                }

                hit = NULL;
                t   = Infinity;
        }
};

}  // namespace RayTracerxx
#endif

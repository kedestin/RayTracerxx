#ifndef BOX_H
#define BOX_H

#include <algorithm>
#include <cmath>
#include <iostream>
#include <utility>
#include "OrderedList.h"
#include "ray.h"

namespace RayTracerxx {

/**
 * @brief      An axis aligned box implemenation
 *
 */
struct Box {
        Number_t low[3], hi[3];
        enum COORDINATES { X = 0, Y, Z };

        Box() : Box(0, 0, 0, 0, 0, 0) {}

        /**
         * @brief      Initializes the box
         *
         * @param[in]  xMax  The x maximum
         * @param[in]  yMax  The y maximum
         * @param[in]  zMax  The z maximum
         * @param[in]  xMin  The x minimum
         * @param[in]  yMin  The y minimum
         * @param[in]  zMin  The z minimum
         */
        Box(Number_t xMax, Number_t yMax, Number_t zMax, Number_t xMin,
            Number_t yMin, Number_t zMin) {
                low[X] = xMin;
                low[Y] = yMin;
                low[Z] = zMin;

                hi[X] = xMax;
                hi[Y] = yMax;
                hi[Z] = zMax;
        }

        /**
         * @brief      Computes the length of the box along dimension dim
         *
         * @param[in]  dim   The dim
         *
         * @return     Number_t length of box
         */
        Number_t d(int dim) const { return hi[dim] - low[dim]; }

        Number_t dX() const { return d(X); }

        Number_t dY() const { return d(Y); }

        Number_t dZ() const { return d(Z); }

        void setMax(int dim, Number_t point) { hi[dim] = point; }

        void setMin(int dim, Number_t point) { low[dim] = point; }

        Number_t volume() const { return dX() * dY() * dZ(); }

        /**
         * @brief      Computes the surface area of a box
         *
         * @return     Number_t area of the box
         */
        Number_t area() const {
                return 2 * dX() * dY() + 2 * dX() * dZ() + 2 * dY() * dZ();
        }

        /**
         * @brief      Checks if box is planar in given dimension
         *
         * @param[in]  k     Dimension
         *
         * @return     True if planar, False otherwise.
         */
        bool isPlanar(int k) const { return (d(k) == 0); }

        /**
         * @brief      Checks whether the box b in contained in *this
         *
         * @param[in]  b     box to check
         *
         * @return     False if any portion of b is not in *this,
         *             True otherwise
         */
        bool contains(const Box& b) const {
                for (int i = 0; i < 3; i++) {
                        if (low[i] > b.low[i] or hi[i] < b.low[i])
                                return false;
                }

                return true;
        }

        friend std::ostream& operator<<(std::ostream& out, const Box& b) {
                return out << "X(" << b.low[X] << "," << b.hi[X] << ") "
                           << "Y(" << b.low[Y] << "," << b.hi[Y] << ") "
                           << "Z(" << b.low[Z] << "," << b.hi[Z] << ") ";
        }

        /**
         * @brief      Finds the entry and exit t for the given Ray
         *
         * @param      r     Ray to Intersect
         *
         * @return     std::pair<Number_t, Number_t>  first  - t_min
         *                                            second - t_max
         *             Returns pair<Ray::Infinity, Ray::Infinity> on miss
         * @Acknowledgement
         *             https://www.scratchapixel.com/lessons/
         *             3d-basic-rendering/
         *             minimal-ray-tracer-rendering-simple-shapes/
         *             ray-box-intersection
         */
        std::pair<Number_t, Number_t> Intersect(Ray& r) const {
                Number_t tmin, tmax, tymin, tymax, tzmin, tzmax;
                // Stores constant in variable to avoid ODR-used linker errors
                Number_t infty = Ray::Infinity;

                // Unrolling this loop was more performant
                tmin = ((r.isNeg[X] ? hi : low)[X] - r.origin[X]) * r.inv(X);
                tmax = ((r.isNeg[X] ? low : hi)[X] - r.origin[X]) * r.inv(X);

                tymin = ((r.isNeg[Y] ? hi : low)[Y] - r.origin[Y]) * r.inv(Y);
                tymax = ((r.isNeg[Y] ? low : hi)[Y] - r.origin[Y]) * r.inv(Y);

                if ((tymin > tymax) || (tmin > tmax))
                        return std::pair<Number_t, Number_t>(infty, infty);
                if (tymin > tmin)
                        tmin = tymin;
                if (tymax < tmax)
                        tmax = tymax;

                tzmin = ((r.isNeg[Z] ? hi : low)[Z] - r.origin[Z]) * r.inv(Z);
                tzmax = ((r.isNeg[Z] ? low : hi)[Z] - r.origin[Z]) * r.inv(Z);

                if ((tzmin > tzmax) || (tmin > tmax))
                        return std::pair<Number_t, Number_t>(infty, infty);

                if (tzmin > tmin)
                        tmin = tzmin;

                if (tzmax < tmax)
                        tmax = tzmax;

                if ((tmax >= 0.0f) && (tmin <= tmax))
                        return std::pair<Number_t, Number_t>(tmin, tmax);
                else
                        return std::pair<Number_t, Number_t>(infty, infty);
        }
};

}  // namespace RayTracerxx

#endif

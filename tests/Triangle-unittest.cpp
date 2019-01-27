#include <gtest/gtest.h>
#include <math.h>
#include <iostream>
#include "PolyObject.h"
#include "ray.h"

TEST(Triangle, Intersect) {
        using RayTracerxx::Ray;
        using RayTracerxx::Triangle;

        Ray ray0({-1.0f, 0.0f, 0.0}, {0.5f, 0.5f, 0.0});
        ray0.direction.normalize();
        Ray ray1({90.0f, 100.0f, -110.0}, {-88.75f, -99.5f, 111.1666});
        ray1.direction.normalize();
        Ray      ray2({1.0f, 1.5f, -0.9}, {1.0f, 0.0f, 0.0});
        Triangle tri0({0.0f, 0.0f, -1.0}, {0.0f, 2.0f, 1.0},
                      {0.0f, 2.0f, -1.0});
        Triangle tri1({1.0f, 0.0f, 1.0}, {1.5f, 0.5f, 1.0},
                      {1.25f, 1.0f, 1.5});

        ray0.t   = Ray::Infinity;
        ray0.hit = nullptr;
        tri0.Intersect(ray0);
        EXPECT_NE(ray0.hit, nullptr) << "ray0 should hit tri0!";
        EXPECT_DOUBLE_EQ(ray0.t, sqrt(2.0f))
            << "ray0 should hit tri0 with a distance of sqrt(2)!";

        ray0.t   = Ray::Infinity;
        ray0.hit = nullptr;
        tri1.Intersect(ray0);
        EXPECT_EQ(ray0.hit, nullptr) << "ray0 should miss tri1!";

        ray1.t   = Ray::Infinity;
        ray1.hit = nullptr;
        tri0.Intersect(ray1);
        EXPECT_EQ(ray1.hit, nullptr) << "ray1 should miss tri0!";

        ray1.t   = Ray::Infinity;
        ray1.hit = nullptr;
        tri1.Intersect(ray1);
        EXPECT_NE(ray1.hit, nullptr) << "ray1 should hit tri1!";
        EXPECT_DOUBLE_EQ(ray1.t, 173.59394980909215)
            << "ray1 should hit tri1 with a distance of 173.593903!";

        ray2.t   = Ray::Infinity;
        ray2.hit = nullptr;
        tri0.Intersect(ray2);
        EXPECT_EQ(ray2.hit, nullptr) << "ray2 should miss tri0!";
}

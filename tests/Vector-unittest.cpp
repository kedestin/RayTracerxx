#include <gtest/gtest.h>
#include <cmath>
#include <iostream>
#include "Box.h"
#include "OrderedList.h"

TEST(Vector, dot) {
        using RayTracerxx::Vector;
        Vector<4> v1{1.0, 2.0, 3.0, 4.0};
        Vector<4> v2{0.0, 1.0, 0.0, 0.0};
        Vector<4> v3{1.0, 1.0, 1.0, 1.0};
        Vector<4> zero{0.0, 0.0, 0.0, 0.0};
        EXPECT_DOUBLE_EQ(v1.dot(v2), 2);
        EXPECT_DOUBLE_EQ(v1.dot(v3), 10);

        EXPECT_DOUBLE_EQ(
            Vector<3>({1.0, -2.0, 3.0}).dot(Vector<3>({5.0, 1.0, -1.0})), 0);

        EXPECT_DOUBLE_EQ(pow(v1.norm(), 2.0), v1.dot(v1));
        EXPECT_DOUBLE_EQ(zero.dot(v1), 0);
        EXPECT_DOUBLE_EQ(zero.dot(zero), 0);
}

TEST(Vector, cross) {
        using RayTracerxx::Vector;

        Vector<3> v1{1, 0, 0};
        Vector<3> v2{0, 1, 0};
        EXPECT_EQ(v1.cross(v2), Vector<3>({0, 0, 1}));

        Vector<3> v3{-1, 0, 0};
        EXPECT_EQ(v2.cross(v3), Vector<3>({0, 0, 1}));

        Vector<3> v4{0, 0, 1};
        EXPECT_EQ(v4.cross(v1), Vector<3>({0, 1, 0}));

        Vector<3> zero{0, 0, 0};
        EXPECT_EQ(zero.cross(v1), zero)
            << "If either vector is the zero vector,"
               " the result is the zero vector.";
        EXPECT_EQ(zero.cross(zero), zero) << "0 x 0 = 0";
}

TEST(Vector, norm) {
        using RayTracerxx::Number_t;
        using RayTracerxx::Vector;

        constexpr unsigned len          = 4;
        Number_t           data[len]    = {0, -3.14, 6.28, -9.42};
        Number_t           expectedNorm = 11.748804194470175;

        Vector<len> v(data);
        EXPECT_DOUBLE_EQ(expectedNorm, v.norm());
}

TEST(Vector, normalize) {
        using RayTracerxx::Vector;

        Vector<4> v({1.0, 2.0, 3.0, 4.0});
        EXPECT_DOUBLE_EQ(1, v.normalize().norm());

        Vector<4> v2({-1.0, -2.0, -3.0, -4.0});
        EXPECT_DOUBLE_EQ(1, v2.normalize().norm());

        Vector<4> v3({-13.0, 332.0, 75.0, .044});
        EXPECT_DOUBLE_EQ(1, v3.normalize().norm());

        Vector<4> v4 = {1, 1, 1, 1};
        EXPECT_EQ(v4.normalize(), Vector<4>({0.5, 0.5, 0.5, 0.5}));

        Vector<3> zero{0, 0, 0};
        EXPECT_DOUBLE_EQ(0, zero.normalize().norm());
}
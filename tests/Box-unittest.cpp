#include "Box.h"
#include <gtest/gtest.h>
#include <iostream>
#include "OrderedList.h"

TEST(Box, Volume) {
        using RayTracerxx::Box;

        EXPECT_DOUBLE_EQ(Box(1, 1, 1, 0, 0, 0).volume(), 1);
        EXPECT_DOUBLE_EQ(Box(2, 2, 2, 1, 1, 1).volume(), 1);
        EXPECT_DOUBLE_EQ(Box(1, 1, 1, 1, 1, 1).volume(), 0);
        EXPECT_DOUBLE_EQ(Box(3.14, 1, 1, 0, 0, 0).volume(), 3.14);
}

TEST(Box, Contains) {}

TEST(Box, Intersect) {
        using RayTracerxx::Box;
        using RayTracerxx::Ray;
        using RayTracerxx::Vector;
        Ray r0({-1.00f, 0.00f, 0.00f}, {0.50f, 0.50f, 0.00f});
        r0.direction.normalize();

        Ray r1({3.00f, 5.00f, 8.00f}, {-2.50f, -4.50f, -7.50f});
        r1.direction.normalize();

        Ray r2({1.25f, 1.25f, 1.25f}, {1.50f, 0.50f, 0.50f});
        r2.direction.normalize();

        Ray r3({1.25f, 1.25f, 1.25f}, {0.00f, 1.00f, 0.00f});
        r3.direction.normalize();

        Box b0(0.50f, 1.00f, 2.00f, 0.00f, 0.00f, 0.00f);
        Box b1(1.50f, 1.50f, 1.50f, 1.00f, 1.00f, 1.00f);

        // Avoid making Ray::Infinity odr-used
        RayTracerxx::Number_t infty = RayTracerxx::Ray::Infinity;

        auto no_hit = std::make_pair(infty, infty);

        EXPECT_NE(b0.Intersect(r0), no_hit) << "r0 should hit b0";
        EXPECT_EQ(b1.Intersect(r0), no_hit) << "r0 shouldn't hit b1";

        EXPECT_NE(b0.Intersect(r1), no_hit) << "r1 should hit b0";
        EXPECT_EQ(b1.Intersect(r1), no_hit) << "r1 shouldn't hit b1";

        EXPECT_EQ(b0.Intersect(r2), no_hit) << "r2 shouldn't hit b0";
        EXPECT_NE(b1.Intersect(r2), no_hit) << "r2 should hit b1";

        EXPECT_NE(b1.Intersect(r3), no_hit) << "r3 should hit b1";

        // EXPECT_DOUBLE_EQ(bi.Insersect(r3), r3. && d == 0.00f, "r3 should hit
        // b1 with a distance of 0.0!" ); TEST( intersects( r0, b0, d ) && d ==
        // sqrt(2.0f), "r0 should hit b0 with a distance of sqrt(2)!" );
}
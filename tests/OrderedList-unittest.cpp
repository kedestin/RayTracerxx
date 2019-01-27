#include "OrderedList.h"
#include <gtest/gtest.h>
#include <stdint.h>
#include <iostream>

TEST(OrderedList, Indexing) {
        using RayTracerxx::Number_t;
        using RayTracerxx::OrderedList;
        Number_t data[100];
        for (int i = 0; i < 100; i++) {
                data[i] = i;
        }

        OrderedList<100> l1(data);
        unsigned         ind = 0;
        for (unsigned i = 0; i < 100; i++) {
                EXPECT_DOUBLE_EQ(data[ind++], l1(i));
        }

        OrderedList<4, 25> l2(data);
        ind = 0;
        for (unsigned i = 0; i < 4; i++) {
                for (unsigned j = 0; j < 25; j++) {
                        EXPECT_DOUBLE_EQ(data[ind++], l2(i, j));
                }
        }
        OrderedList<2, 2, 25> l3(data);
        ind = 0;
        for (unsigned i = 0; i < 2; i++) {
                for (unsigned j = 0; j < 2; j++) {
                        for (unsigned k = 0; k < 25; k++) {
                                EXPECT_DOUBLE_EQ(data[ind++], l3(i, j, k));
                        }
                }
        }
        OrderedList<2, 2, 5, 5> l4(data);
        ind = 0;
        for (unsigned i = 0; i < 2; i++) {
                for (unsigned j = 0; j < 2; j++) {
                        for (unsigned k = 0; k < 5; k++) {
                                for (unsigned l = 0; l < 5; l++) {
                                        EXPECT_DOUBLE_EQ(data[ind++],
                                                         l4(i, j, k, l));
                                }
                        }
                }
        }

        // Out of bounds
        EXPECT_THROW(l1(static_cast<unsigned>(1000)), std::logic_error);
        EXPECT_THROW(l2(static_cast<unsigned>(5), static_cast<unsigned>(26)),
                     std::logic_error);
        EXPECT_THROW(l3(static_cast<unsigned>(3), static_cast<unsigned>(0),
                        static_cast<unsigned>(0)),
                     std::logic_error);
        EXPECT_THROW(l4(static_cast<unsigned>(0), static_cast<unsigned>(0),
                        static_cast<unsigned>(0), static_cast<unsigned>(342)),
                     std::logic_error);
}
TEST(OrderedList, Elementwise_Addition) {
        using RayTracerxx::Number_t;
        using RayTracerxx::OrderedList;
        constexpr unsigned size        = 5;
        Number_t           data1[size] = {1, 2, 4, 5, 6};
        Number_t           data2[size] = {3, 7, 9, 15, 21};
        Number_t           dataresult[size];

        for (unsigned i = 0; i < size; i++) {
                dataresult[i] = data1[i] + data2[i];
        }
        OrderedList<size> l1(data1);
        OrderedList<size> l2(data2);
        OrderedList<size> result(dataresult);

        EXPECT_EQ(result, l1 + l2);
}
TEST(OrderedList, Elementwise_Multiplication) {
        using RayTracerxx::Number_t;
        using RayTracerxx::OrderedList;
        constexpr unsigned size        = 5;
        Number_t           data1[size] = {1, 2, 4, 5, 6};
        Number_t           data2[size] = {3, 7, 9, 15, 21};
        Number_t           dataresult[size];

        for (unsigned i = 0; i < size; i++) {
                dataresult[i] = data1[i] * data2[i];
        }
        OrderedList<size> l1(data1);
        OrderedList<size> l2(data2);
        OrderedList<size> result(dataresult);

        EXPECT_EQ(result, l1 * l2);
}
TEST(OrderedList, Elementwise_Subtraction) {
        using RayTracerxx::Number_t;
        using RayTracerxx::OrderedList;
        constexpr unsigned size        = 5;
        Number_t           data1[size] = {1, 2, 4, 5, 6};
        Number_t           data2[size] = {3, 7, 9, 15, 21};
        Number_t           dataresult[size];

        for (unsigned i = 0; i < size; i++) {
                dataresult[i] = data1[i] - data2[i];
        }
        OrderedList<size> l1(data1);
        OrderedList<size> l2(data2);
        OrderedList<size> result(dataresult);

        EXPECT_EQ(result, l1 - l2);
}
TEST(OrderedList, Addition_with_Constant) {
        using RayTracerxx::Number_t;
        using RayTracerxx::OrderedList;
        constexpr unsigned size        = 5;
        Number_t           data1[size] = {1, 2, 4, 5, 6};
        Number_t           toAdd       = 9;
        Number_t           dataresult[size];

        for (unsigned i = 0; i < size; i++) {
                dataresult[i] = data1[i] + toAdd;
        }
        OrderedList<size> l1(data1);
        OrderedList<size> result(dataresult);

        EXPECT_EQ(result, l1 + toAdd);
}
TEST(OrderedList, Multiplication_with_Constant) {
        using RayTracerxx::Number_t;
        using RayTracerxx::OrderedList;
        constexpr unsigned size        = 5;
        Number_t           data1[size] = {1, 2, 4, 5, 6};
        Number_t           toMult      = 9;
        Number_t           dataresult[size];

        for (unsigned i = 0; i < size; i++) {
                dataresult[i] = data1[i] * toMult;
        }
        OrderedList<size> l1(data1);
        OrderedList<size> result(dataresult);

        EXPECT_EQ(result, l1 * toMult);
}
TEST(OrderedList, Subtraction_with_Constant) {
        using RayTracerxx::Number_t;
        using RayTracerxx::OrderedList;
        constexpr unsigned size        = 5;
        Number_t           data1[size] = {1, 2, 4, 5, 6};
        Number_t           toSub       = 9;
        Number_t           dataresult[size];

        for (unsigned i = 0; i < size; i++) {
                dataresult[i] = data1[i] - toSub;
        }
        OrderedList<size> l1(data1);
        OrderedList<size> result(dataresult);

        EXPECT_EQ(result, l1 - toSub);
}

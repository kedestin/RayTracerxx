#ifndef ORDEREDLIST_H
#define ORDEREDLIST_H

#include <assert.h>
#include <math.h>
#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <type_traits>
namespace RayTracerxx {

// Number type for data
typedef double Number_t;

/**
 * Abstractions defined in this file
 *
 *     OrderedList
 *     Point
 *     Matrix
 *     Vector
 *
 */

/**
 * @brief      Represention of arbitrarily shaped lists of numbers
 *
 * @tparam     sizes  List dimensions
 */
template <unsigned... sizes>
class OrderedList {
        static_assert(sizeof...(sizes) > 0,
                      "Cannot have 0 template parameters");

protected:
        /*
         *                     Template Metaprogramming
         */

        /**
         * @brief      Metafunction that multiplies all elements of parameter
         *             pack
         *
         * @tparam     list  List of unsigned numbers
         */
        template <unsigned... list>
        struct multiply {
                enum { value = 1 };
        };

        template <unsigned first, unsigned... tail>
        struct multiply<first, tail...> {
                enum { value = first * multiply<tail...>::value };
        };

        /**
         * @brief      Stub functions for compile-time type resolution
         *
         * @details    Any argument that is derived from OrderedList matches
         *             the version that returns true type. All others match
         *             the variadic version.
         *             This works for any specialization of OrderedList or
         *             its derived classes
         *
         * @acknowledgement
         *             https://stackoverflow.com/questions/51910808/
         *             detect-is-base-of-with-base-class-template
         *
         */
        static std::false_type OL_pattern_match(...);
        template <unsigned... args>
        static std::true_type OL_pattern_match(const OrderedList<args...>&);
        template <typename T>
        using is_OrderedList = decltype(OL_pattern_match(std::declval<T&>()));

        /**
         * @brief      Variadic version of std::is_same
         */
        template <class... args>
        struct is_same_variadic {
                enum { value = true };
        };

        template <class a, class b, class... rest>
        struct is_same_variadic<a, b, rest...> {
                enum { value = std::is_same<a, b>::value &&
                               is_same_variadic<b, rest...>::value };
        };

        /**
         * @brief      Helper struct to convert list of indices to 1D index
         *
         * @details    Each struct is a "node" in a pseudo linked list.
         *             The struct in initialize with the sizes of the Ordered
         *             List and recursively builds the list at compile time.
         *             The i-th node's value is the product of the ith - last
         *             elements of the sizes list. The base base has a value of
         *             1
         *
         *             Computing the 1D index is then the linear combination
         *             of the elements in the pseudo linked list (skipping
         *             the first one) and the requested indices
         *
         *             ex: OrderedList<3,2,2>
         *                 struct linearIndex := 12 ->   4   ->  2    -> 1
         *                 convert(2, 1, 1)   :=     2 * 4 + 1 * 2 + 1 * 1 + 0
         *                 Note: The first value of the linearIndex list
         *                       is skipped
         * @tparam     i     { description }
         */
        template <unsigned... i>
        struct linearIndex {
                enum { value = 1 };
                static constexpr unsigned convert() { return 0; }
        };

        template <unsigned first, unsigned... rest>
        struct linearIndex<first, rest...> {
                enum { value = first * linearIndex<rest...>::value };
                typedef linearIndex<rest...> next;

                template <class T, class... S>
                static constexpr typename std::enable_if<
                    std::is_same<unsigned, T>::value &&
                        is_same_variadic<unsigned, S...>::value,
                    unsigned>::type
                convert(const T& front, const S... args) {
                        return next::value * (front) + next::convert(args...);
                }
        };

        /**
         * @brief      Checks that indices are valid
         *
         * @param[in]  indices  The indices
         */
        void assertInRange(const unsigned* indices) const {
                constexpr unsigned size[sizeof...(sizes)] = {sizes...};
                for (unsigned long i = 0; i < sizeof...(sizes); i++)
                        if (not(indices[i] < size[i]))
                                throw std::logic_error(
                                    "Error: Index out "
                                    "of range\n");
        }

public:
        typedef multiply<sizes...> numElements;
        static_assert(numElements::value != 0,
                      "All list dimensions must be greater than 0");
        // Prevent size 0 arrays

protected:
        Number_t data[numElements::value];

public:
        OrderedList() : data{0} {};
        OrderedList(const Number_t newData[numElements::value]) {
                for (unsigned i = 0; i < numElements::value; i++)
                        data[i] = newData[i];
        }

        bool operator==(const OrderedList<sizes...>& toComp) const {
                if (&toComp == this)
                        return true;

                for (unsigned i = 0; i < numElements::value; i++)
                        if (data[i] != toComp.data[i])
                                return false;

                return true;
        }

        /*
         *                      Arithmetic Overload Set
         *                             +, -, *
         * @brief      Overload set defines elementwise
         *             (OrderedList - OrderedList) operations and operations
         *             with a constant operand (OrderedList - constant)
         *
         *             Element wise operations are only defined on lists of
         *             same dimension
         *
         * @details    Operations on rvalues and self assignment operations
         *             avoid unnecessary copies.
         *             So prefer longer expressions of +, -, * or multiple
         *             self assignments to multiple short expressions of
         *             +, -, *.
         *
         *             ex:  list = list2 + list3   vs  list = list + list2
         *                                             list = list + list3
         *                          or
         *
         *                  list += list2
         *                  list += list3
         *
         *
         */
        template <class T>
        typename std::enable_if<not is_OrderedList<T>::value,
                                OrderedList&>::type
        operator+=(const T& toAdd) {
                for (unsigned i = 0; i < numElements::value; i++)
                        data[i] += toAdd;
                return *this;
        }
        OrderedList& operator+=(const OrderedList<sizes...>& toAdd) {
                for (unsigned i = 0; i < numElements::value; i++)
                        data[i] += toAdd.data[i];
                return *this;
        }
        template <class T>
        typename std::enable_if<not is_OrderedList<T>::value,
                                OrderedList&>::type
        operator-=(const T& toAdd) {
                for (unsigned i = 0; i < numElements::value; i++)
                        data[i] -= toAdd;

                return *this;
        }
        OrderedList& operator-=(const OrderedList<sizes...>& toAdd) {
                for (unsigned i = 0; i < numElements::value; i++)
                        data[i] -= toAdd.data[i];

                return *this;
        }

        template <class T>
        typename std::enable_if<not is_OrderedList<T>::value,
                                OrderedList&>::type
        operator*=(const T& toAdd) {
                for (unsigned i = 0; i < numElements::value; i++)
                        data[i] *= toAdd;

                return *this;
        }

        OrderedList& operator*=(const OrderedList<sizes...>& toAdd) {
                for (unsigned i = 0; i < numElements::value; i++)
                        data[i] *= toAdd.data[i];

                return *this;
        }

        template <class T>
        OrderedList operator+(const T& toAdd) const& {
                return OrderedList(*this) += toAdd;  // make a copy
        }

        template <class T>
        OrderedList operator-(const T& toAdd) const& {
                return OrderedList(*this) -= toAdd;  // make a copy
        }

        template <class T>
        OrderedList operator*(const T& toAdd) const& {
                return OrderedList(*this) *= toAdd;  // make a copy
        }

        template <class T>
        OrderedList&& operator+(const T& toAdd) && {
                return static_cast<OrderedList&&>(*this += toAdd);
        }
        template <class T>
        OrderedList&& operator-(const T& toAdd) && {
                return static_cast<OrderedList&&>(*this -= toAdd);
        }

        template <class T>
        OrderedList&& operator*(const T& toAdd) && {
                return static_cast<OrderedList&&>(*this *= toAdd);
        }

        // OrderedList && operator+(OrderedList&& toAdd) const&{
        //         return static_cast<OrderedList&&>(toAdd += *this);
        // }

        // OrderedList && operator-(OrderedList&& toAdd) const&{
        //         return static_cast<OrderedList&&>(toAdd -= *this);
        // }

        // OrderedList && operator*(OrderedList&& toAdd) const&{
        //         return static_cast<OrderedList&&>(toAdd *= *this);
        // }

        friend std::ostream& operator<<(std::ostream&                stream,
                                        const OrderedList<sizes...>& list) {
                int format[sizeof...(sizes)] = {sizes...};
                int size[sizeof...(sizes)]   = {sizes...};

                for (unsigned i = 0; i < numElements::value; i++) {
                        stream << list.data[i] << " ";
                        format[0]--;

                        for (unsigned long j = 0; j < sizeof...(sizes) - 1;
                             j++) {
                                if (format[j] == 0) {
                                        format[j] = size[j];
                                        format[j + 1]--;
                                        stream << "\n";
                                }
                        }
                }
                return stream;
        }

        template <class... T>
        Number_t& operator()(T... args) {
                static_assert(
                    sizeof...(args) == sizeof...(sizes),
                    "List indices must match dimensionality of list");
                static_assert(is_same_variadic<unsigned, T...>::value ||
                                  is_same_variadic<int, T...>::value,
                              "Indices must be integers");
                // Implicit conversion to unsigned integer
                const unsigned currIndex[sizeof...(args)] = {args...};
                assertInRange(currIndex);

                return data[linearIndex<sizes...>::convert(args...)];
        }
};

/**
 * @brief      Geometric point representation
 *
 * @tparam     dimen  Number of spatial dimensions
 */
template <unsigned dimen>
class Point : public OrderedList<dimen> {
public:
        using OrderedList<dimen>::data;
        Point() : OrderedList<dimen>() {}
        Point(const OrderedList<dimen>& toCopy) : OrderedList<dimen>(toCopy) {}
        Point(const std::initializer_list<Number_t>& i)
            : OrderedList<dimen>(std::begin(i)) {}
        Point(const Number_t newData[dimen]) : OrderedList<dimen>(newData){};

        const Number_t& operator[](unsigned ind) const {
                OrderedList<dimen>::assertInRange(&ind);
                return data[ind];
        }
        Number_t& operator[](unsigned ind) {
                OrderedList<dimen>::assertInRange(&ind);
                return data[ind];
        }
};

/**
 * @brief      Matrix representation
 *
 * @tparam     sizes  Matrix dimensions
 */
template <unsigned... sizes>
class Matrix : public OrderedList<sizes...> {
public:
        static_assert(sizeof...(sizes) <= 2, "Matrix must be of form m x n");
        using typename OrderedList<sizes...>::numElements;
        Matrix() : OrderedList<sizes...>() {}
        Matrix(const Number_t d[numElements::value])
            : OrderedList<sizes...>(d) {}
        Matrix(const OrderedList<sizes...>& ol) : OrderedList<sizes...>(ol) {}

        using OrderedList<sizes...>::data;
};

/**
 * @brief      Geometric vector representation
 *
 * @tparam     dimen  Number of spatial dimensions
 */
template <unsigned dimen>
class Vector : public Matrix<dimen> {
public:
        Vector() : Matrix<dimen>() {}
        Vector(const Number_t d[dimen]) : Matrix<dimen>(d) {}
        Vector(const std::initializer_list<Number_t> l)
            : Matrix<dimen>(std::begin(l)) {}
        Vector(const OrderedList<dimen>& toCopy) : Matrix<dimen>(toCopy) {}

        using Matrix<dimen>::data;

        /**
         * @brief      Computes the dot product of two vectors
         *
         * @param[in]  toDot  To dot
         *
         * @return     Dot product of the two vectors
         */
        Number_t dot(const Vector<dimen>& toDot) const {
                Number_t result = 0;
                for (unsigned i = 0; i < dimen; i++)
                        result += data[i] * toDot.data[i];
                return result;
        }

        /**
         * @brief      Computes the cross product of two vectors
         *
         * @param[in]  toCross  Vector to cross with
         *
         * @return     the result of the cross product
         *
         * @note       Currently only supports 3 dimensional vectors
         */
        Vector<dimen> cross(const Vector<dimen>& toCross) const {
                static_assert(dimen == 3,
                              "Only supports 3 "
                              "dimensional vectors");

                return {data[1] * toCross[2] - data[2] * toCross[1],
                        data[2] * toCross[0] - data[0] * toCross[2],
                        data[0] * toCross[1] - data[1] * toCross[0]};
        }

        /**
         * @brief      Gets the magnitude of the vector
         *
         * @return     Magnitude
         */
        Number_t norm() const {
                Number_t squaredSum = 0;

                for (const Number_t& i : data)
                        squaredSum += i * i;

                return sqrt(squaredSum);
        }

        /**
         * @brief      Normalizes the vector to a magnitude of 1
         *
         * @return     A reference to the vector
         *
         * @Note       The zero vector will remain the zero vector after
         *             normalization
         */
        Vector& normalize() {
                Number_t m = norm();

                if (m != 0)  // Zero vector cannot be normalized
                        for (Number_t& i : data)
                                i /= m;

                return *this;
        }

        const Number_t& operator[](unsigned ind) const {
                Matrix<dimen>::assertInRange(&ind);
                return data[ind];
        }

        Number_t& operator[](unsigned ind) {
                Matrix<dimen>::assertInRange(&ind);
                return data[ind];
        }
};

}  // namespace RayTracerxx

#endif
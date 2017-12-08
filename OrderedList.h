#ifndef ORDEREDLIST_H
#define ORDEREDLIST_H

#include <iostream>

using namespace std;
class OrderedList{
public:
	OrderedList(float*, int* size,  int dimension);
	OrderedList(initializer_list<float>, initializer_list<int>, int);
	OrderedList(const OrderedList& toCopy);
	~OrderedList();

	void operator=(const OrderedList&);
	bool operator==(const OrderedList&);

	OrderedList operator+(const OrderedList&);
	OrderedList operator+(float);
	OrderedList operator+(double);
	OrderedList operator+(int);

	OrderedList operator-(const OrderedList&);
	OrderedList operator-(float);
	OrderedList operator-(double);
	OrderedList operator-(int);

	OrderedList operator*(const OrderedList&);
	OrderedList operator*(float);
	OrderedList operator*(double);
	OrderedList operator*(int);
	float&      operator()(initializer_list<int> );

	friend ostream &operator<<(ostream &stream, const OrderedList& list);

	int dimensions;
	int numElements;
	int* size;
	float *data;

private:
	friend class Vector;
	friend class Point;

	const static int STATIC_MEMORY = 10;
	float constantData[STATIC_MEMORY];
	int   constantSize[STATIC_MEMORY];

	void copyData(float*, int* size,  int dimension);
        void assertSize(const OrderedList& toCheck);
        void assertInRange(const int* indices);
};

class Point: public OrderedList{
public:
	Point(): OrderedList((float *) begin( (initializer_list<float>){ 0 } ),
	                     (int *)   begin( (initializer_list<int>){ 1, 1 } ),
	                               2){}
	Point(const OrderedList& toCopy) : OrderedList(toCopy) {}
	Point(float* data , int dim): OrderedList(data, 
		             (int *) begin( (initializer_list<int>){ 1, dim } ),
		             2){}

	Point(initializer_list<float> newData, int dimension) : OrderedList(
		                                                newData,
		                                                {1, dimension},
		                                                 2) {}

	float& operator[](int ind);
};

class Matrix: public OrderedList{
public:
	Matrix(float* data, int* size) : OrderedList(data, size, 2) {}

	Matrix(initializer_list<float> newData, initializer_list<int> size) :
		 OrderedList(newData, size, 2){}
	
	Matrix(const OrderedList& toCopy) : OrderedList(toCopy) {}

};

class Vector: public Matrix{
public:
	Vector() : Vector((float *) begin((initializer_list<float>) {0}), 1){}
	Vector(float* data, int dimen): Matrix(data,
	                    (int *) begin((initializer_list<int>){1, dimen})) {}
	Vector(initializer_list<float> newData, int dimension) : Matrix(newData,
	                                                     {1, dimension}) {}
	Vector(const OrderedList& toCopy) : Matrix(toCopy) {}

	float  dot(const Vector&);
	Vector cross(const Vector&);
	float& operator[](int ind);
	float  magnitude();
	void   normalize();

};

#endif
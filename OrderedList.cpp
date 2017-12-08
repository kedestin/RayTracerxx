#include "OrderedList.h"
#include "math.h"
#include <iostream>
#include <stdexcept>


using namespace std;

OrderedList::OrderedList(float* newData, int* newSize, int dimen){
	numElements = 1;
	dimensions = dimen;
	size = (dimensions <= STATIC_MEMORY) ? constantSize 
	                                     : new int[dimensions];

	for (int i = 0; i < dimen; i++){
		if (newSize[i] <= 0){
			throw runtime_error("Size must be > 0");
		}

		numElements *= newSize[i];
		size[i] = newSize[i];
	}

	data = (numElements <= STATIC_MEMORY) ? constantData 
	                                      : new float[numElements];


	for (int i = 0; i < numElements; i++)
		data[i] = newData[i];

}


OrderedList::OrderedList(initializer_list<float>  newData,
                         initializer_list<int>    newSize,
                         int                      dimen) 
                       : OrderedList((float*)     begin(newData),
                                     (int*)       begin(newSize),
                                                  dimen) {}


OrderedList::OrderedList(const OrderedList& toCopy){
	numElements = toCopy.numElements;
	dimensions = toCopy.dimensions;
	size = (dimensions <= STATIC_MEMORY)  ? constantSize 
	                                      : new int[dimensions];
	data = (numElements <= STATIC_MEMORY) ? constantData
	                                      : new float[numElements];


	for (int i = 0; i < dimensions; i++)
		size[i] = toCopy.size[i];

	for (int i = 0; i < numElements; i++)
		data[i] = toCopy.data[i];

}

OrderedList::~OrderedList(){
	if (size != constantSize)
		delete [] size;
	if (data != constantData)
		delete [] data;

}


void OrderedList::operator=(const OrderedList& toCopy){
	if (&toCopy == this)
		return;

	if (numElements != toCopy.numElements){ 
		if (data != constantData)
			delete [] data;
		numElements = toCopy.numElements;
		data = (numElements <= STATIC_MEMORY) ? constantData
		                                      : new float[numElements];		
	}

	if (dimensions != toCopy.dimensions){
		if (size != constantSize)
			delete [] size;
		dimensions = toCopy.dimensions;
		size = (dimensions <= STATIC_MEMORY) ? constantSize
		                                     : new int[dimensions];
	}

	for (int i = 0; i < dimensions; i++)
		size[i] = toCopy.size[i];

	for (int i = 0; i < numElements; i++)
		data[i] = toCopy.data[i];
}

bool OrderedList::operator==(const OrderedList& toComp){
	if (&toComp == this)
		return true;

	if (dimensions != toComp.dimensions)
		return false;

	if (numElements != toComp.numElements)
		return false;

	for (int i = 0; i < numElements; i++)
		if (data[i] != toComp.data[i])
			return false;

	return true;


}

OrderedList OrderedList::operator+(const OrderedList& toAdd){
	assertSize(toAdd);

	OrderedList tempList(*this);

	for (int i = 0; i < numElements; i++)
		tempList.data[i] += toAdd.data[i];

	return tempList;
}

OrderedList OrderedList::operator+(float toAdd){

	OrderedList tempList(*this);

	for (int i = 0; i < numElements; i++)
		tempList.data[i] += toAdd;

	return tempList;
}

OrderedList OrderedList::operator+(double toAdd){
	OrderedList tempList(*this);

	for (int i = 0; i < numElements; i++)
		tempList.data[i] += toAdd;

	return tempList;
}

OrderedList OrderedList::operator+(int toAdd){
	OrderedList tempList(*this);

	for (int i = 0; i < numElements; i++)
		tempList.data[i] += toAdd;	

	return tempList;
}

OrderedList OrderedList::operator-(const OrderedList& toRemove){
	assertSize(toRemove);

	OrderedList tempList(*this);

	for (int i = 0; i < numElements; i++)
		tempList.data[i] -= toRemove.data[i];

	return tempList;
}

OrderedList OrderedList::operator-(float toRemove){
	OrderedList tempList(*this);

	for (int i = 0; i < numElements; i++)
		tempList.data[i] -= toRemove;

	return tempList;
}

OrderedList OrderedList::operator-(double toRemove){
	OrderedList tempList(*this);

	for (int i = 0; i < numElements; i++)
		tempList.data[i] -= toRemove;

	return tempList;
}

OrderedList OrderedList::operator-(int toRemove){
	OrderedList tempList(*this);

	for (int i = 0; i < numElements; i++)
		tempList.data[i] -= toRemove;

	return tempList;
}

OrderedList OrderedList::operator*(const OrderedList& toMult){
	assertSize(toMult);

	OrderedList tempList(*this);

	for (int i = 0; i < numElements; i++)
		tempList.data[i] *= toMult.data[i];

	return tempList;
}

OrderedList OrderedList::operator*(float toMult){
	OrderedList tempList(*this);

	for (int i = 0; i < numElements; i++)
		tempList.data[i] *= toMult;

	return tempList;
}

OrderedList OrderedList::operator*(double toMult){
	OrderedList tempList(*this);

	for (int i = 0; i < numElements; i++)
		tempList.data[i] *= toMult;

	return tempList;
}

OrderedList OrderedList::operator*(int toMult){
	OrderedList tempList(*this);

	for (int i = 0; i < numElements; i++)
		tempList.data[i] *= toMult;	

	return tempList;
}

ostream &operator<<(ostream &stream, const OrderedList& list){
	int* format = new int[list.dimensions];

	for (int i = 0; i < list.dimensions; i++)
		format[i] = list.size[i];

	for (int i = 0; i < list.numElements; i++){
		stream << list.data[i] <<" "; 
		format[0]--;


		for (int j = 0; j < list.dimensions-1; j++){
			if (format[j] == 0){
				format[j] = list.size[j];
				format[j+1]--;
				stream << "\n";
			}
		}
	}

	delete [] format;
	return stream;
}

float& OrderedList::operator()(initializer_list<int> indices){
	const int* currIndex = begin(indices);
	
	assertInRange(begin(indices));

	int linearIndex = currIndex[0], offset = size[0];

	for (int i = 1; i < dimensions; i++){
		linearIndex += currIndex[i] *  offset;
		offset *= size[i];
	}

	return data[linearIndex];
}

void OrderedList::assertInRange(const int* indices){
	for (int i = 0; i < dimensions; i++)
		if (not ( 0 <= indices[i] and indices[i] < size[i]) )
			throw runtime_error("Error: Index out of range\n");
}
void OrderedList::assertSize(const OrderedList& toCheck){
	if (dimensions != toCheck.dimensions)
		throw runtime_error("Error: Missmatched Matrix Dimensions\n");

	for (int i = 0; i < dimensions; i++)
		if (toCheck.size[i] != size[i])

			throw runtime_error("Error: Matrix Size Missmatch\n");
}


float Vector::dot(const Vector& toDot){
	assertSize(toDot);

	float result = 0;

	for (int i = 0; i < size[1]; i++){
		result += data[i] * toDot.data[i];
	}

	return result; 
}

Vector Vector::cross(const Vector& toCross){
	if (size[1] != 3 or dimensions != 2)
		throw runtime_error("Only supports 3-D Vectors");

	assertSize(toCross);
	Vector temp(toCross);

	float cross[3] = {data[1] * temp[2] - data[2] * temp[1], 
			  data[2] * temp[0] - data[0] * temp[2],
			  data[0] * temp[1] - data[1] * temp[0] }; 

	return	Vector(cross, 3);
}

float Vector::magnitude(){
	float squaredSum = 0;

	for (int i = 0; i < numElements; i++)
		squaredSum +=  data[i] * data[i];

	return sqrt(squaredSum);
}

void Vector::normalize(){
	float norm = this->magnitude();

	for (int i = 0; i < numElements; i++)
		data[i] /= norm;

}


float& Vector::operator[](int ind){
	int *index = (int*) (begin((initializer_list<int>) {0, ind}));
	assertInRange(index);
	
	return data[ind];
}

float& Point::operator[](int ind){
	int *index = (int*) (begin((initializer_list<int>) {0, ind}));
	assertInRange(index);
	
	return data[ind];
}
#include <iostream>
#include "OrderedList.h"

using namespace std;

void runTests(OrderedList &testlist);

int main(){

	int listSize = 10;
	float list[listSize];
	int size[2] = {1, 10};
	for (int i = 0; i < listSize; i++){
		list[i] = (float) i;
	}

	OrderedList testlist(list, size, 2);

	runTests(testlist);

	Point testPoint(list, listSize);
	runTests(testPoint);

	Vector testVec(list, listSize);
	runTests(testVec);


	cout << testVec.dot(testVec);

	testlist({0, 3}) = 99;

	cout << testlist;
	return 0;
}

void runTests(OrderedList &testlist){ 
	cout << testlist <<endl;

	cout << testlist( {0, 3} ) <<endl; //Only works post c++11

	testlist = testlist + testlist;

	cout <<testlist <<endl;

	testlist = testlist * testlist;

	cout << testlist <<endl;

	testlist = testlist - testlist;

	cout << testlist <<endl;

	testlist = testlist + 5.2;

	cout << testlist <<endl;

	testlist = testlist * 2.1;

	cout << testlist << endl;

	testlist = testlist - 1.11;

	cout <<testlist <<endl;
}
#
#
#  COMP 15 Proj 2 RayTracer++ - Reverse Polish Notation Arithmetic
#
#  Makefile
# 
#  Modified By kdesti01:
#           On 10/2/2017:
#
#

CXX      = clang++
CXXFLAGS = -g3 -std=c++11 -Wall -Wextra
LDFLAGS  = -g3

RayTracer++: main.o OrderedList.o Camera.o Scene.o ImageEngine.o OrderedList.o  KDTree.o 
	${CXX} ${LDFLAGS} $^ -std=c++11 -o RayTracer++ tinyply/source/tinyply.cpp

main.o:			main.cpp 
OrderedList.o:		OrderedList.cpp OrderedList.h
Camera.o:		Camera.cpp Camera.h
ImageEngine.o:		ImageEngine.cpp ImageEngine.h
Scene.o:		Scene.cpp Scene.h
testOrderedList.o:	testOrderedList.cpp OrderedList.h
testCamera.o:		testCamera.cpp Camera.h
testScene.o:		testScene.cpp Scene.h
tinyply.o:		tinyply/source/tinyply.cpp tinyply/source/tinyply.h
KDTree.o:		KDTree.cpp KDTree.h

clean:
	rm -rf RayTracer++ *.o *.dSYM

run: 
	make && ./RayTracer++

valgrind: 
	make && valgrind ./RayTracer++

dependencies:
	make OrderedList.o testOrderedList.o  Camera.o testCamera.o \
	     testCamera.o Scene.o ImageEngine.o testScene.o tinyply.o KDTree.o

testStack:
	make dependencies
	${CXX} ${CXXFLAGS} testOrderedList.o OrderedList.o   && ./a.out

testCamera:
	make dependencies
	${CXX} ${CXXFLAGS} testCamera.o OrderedList.o  Camera.o  \
		&& ./a.out

testScene:
	make dependencies
	${CXX} ${CXXFLAGS} testScene.o Scene.o ImageEngine.o OrderedList.o  Camera.o tinyply/source/tinyply.cpp KDTree.o  \
		&& ./a.out		

testSceneValgrind:
	make dependencies
	${CXX} ${CXXFLAGS} testScene.o Scene.o ImageEngine.o OrderedList.o  Camera.o  \
		&& valgrind ./a.out

testStackValgrind:
	make dependencies
	${CXX} ${CXXFLAGS} testOrderedList.o OrderedList.o  && valgrind ./a.out

testCameraValgrind:
	make dependencies
	${CXX} ${CXXFLAGS} testCamera.o OrderedList.o  Camera.o   \
		&& valgrind ./a.out

provide:
	provide comp15 proj1 OrderedList.h OrderedList.cpp \
				testOrderedList.cpp Camera.h Camera.cpp \
				testCamera.cpp main.cpp README Makefile \
				LinkedList.h emptyTests nonEmptyTest 

# Camera.cpp
# Camera.h
# ImageEngine.cpp
# ImageEngine.h
# KDTree.cpp
# KDTree.h
# main.cpp
# Makefile
# OrderedList.cpp
# OrderedList.h
# PolyObject.h
# ray.h
# README
# rgb.h
# Scene.cpp
# Scene.h
# testCamera.cpp
# testOrderedList.cpp
# testRayTracer
# testScene.cpp
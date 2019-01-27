#!/bin/sh


# Download some example ply files

echo "Downloading example ply files"
echo ""
if ! [ -f assets/sphere.ply ]; then
        wget https://people.sc.fsu.edu/~jburkardt/data/ply/sphere.ply -P assets
fi

if ! [ -f assets/street_lamp.ply ]; then
        wget https://people.sc.fsu.edu/~jburkardt/data/ply/street_lamp.ply -P assets
fi

if ! [ -f assets/cow.ply ]; then
        wget https://people.sc.fsu.edu/~jburkardt/data/ply/cow.ply -P assets
fi

if ! [ -f assets/beethoven.ply ]; then
        wget https://people.sc.fsu.edu/~jburkardt/data/ply/beethoven.ply -P assets
fi


# Compile executable and tests
echo ""
echo "Compiling executables"
echo ""
if ! make; then
        echo "Building RayTracerxx failed"
        exit 1
fi

if ! make unittests; then
        echo "Building unit tests failed"
        exit 1
fi


echo ""
echo "Running unittests"
echo ""
# Run unittests
./unittests

# Render examples



OUTPUT=output
if ! [ -d $OUTPUT ]; then
        mkdir $OUTPUT
fi

echo ""
echo "Rendering sphere.ply"
echo ""

./RayTracer++ << ENDOFCOMMANDS
newScene 1920 1080
newObject ./assets/sphere.ply
setPosition -8 4 304
newLight 1.000000 0.000000 0.000000 2147483647 0 0
newLight 1.000000 1.000000 0.000000 2147483647 0 0
newLight 0.000000 1.000000 0.000000 0 2147483647 0
render $OUTPUT/sphere.ppm
ENDOFCOMMANDS

echo ""
echo "Rendering street_lamp.ply"
echo ""

./RayTracer++ << ENDOFCOMMANDS
newScene 1920 1080
newObject ./assets/street_lamp.ply
setPosition 4 2.5 11
newLight 1.000000 0.000000 0.000000 2147483647 0 0
newLight 1.000000 1.000000 0.000000 2147483647 0 0
newLight 0.000000 1.000000 0.000000 0 2147483647 0
render $OUTPUT/street_lamp.ppm
ENDOFCOMMANDS


echo ""
echo "Rendering cow.ply"
echo ""

./RayTracer++ << ENDOFCOMMANDS
newScene 1920 1080
newObject ./assets/cow.ply
setPosition -0.375 1.625 3.5
newLight 1.000000 0.000000 0.000000 2147483647 0 0
newLight 1.000000 1.000000 0.000000 2147483647 0 0
newLight 0.000000 1.000000 0.000000 0 2147483647 0
render $OUTPUT/cow.ppm
ENDOFCOMMANDS


echo ""
echo "Rendering beethoven.ply"
echo ""

./RayTracer++ << ENDOFCOMMANDS
newScene 1920 1080
newObject ./assets/beethoven.ply
setPosition -4 0 14
newLight 1.000000 0.000000 0.000000 2147483647 0 0
newLight 1.000000 1.000000 0.000000 2147483647 0 0
newLight 0.000000 1.000000 0.000000 0 2147483647 0
render $OUTPUT/beethoven.ppm
ENDOFCOMMANDS

echo ""
echo "Check output in $OUTPUT folder"
echo ""
unset OUTPUT
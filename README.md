
# RayTracer++

## What is RayTracer++

RayTracer++ is a simple scene description language that uses accelerated
raytracing to render scenes. Users can import triangle meshes from .ply files, preview the scene on the terminal, and render the image.

## Quick start

This repository includes a setup script `setup.sh` that will:

 * Download example ply files
 * Compile unittests and project
 * Run unittests
 * Render example ply files
 
 ## Running unit tests
 
 Unit tests depend on [Google Test](https://github.com/google/googletest)
 
 Build unit tests by running `make unittests`. Makefile assumes that Google Test includes are located in `usr/include/` and the static library is located at `/usr/lib`. These can be overriden by specifying them at compile time `make GTEST_INCLUDE=PATH/TO/INCLUDE GTEST_LIB=PATH/TO/LIB unittests`.
 

#!/bin/bash
mkdir -p build
cd build
cmake -DGLFW_BUILD_DOCS=OFF -S ../ -B .
make && make Shaders && ./BrainEngine
cd ..

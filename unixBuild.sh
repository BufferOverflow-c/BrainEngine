#!/bin/bash
mkdir -p build
cd build
cmake -DGLFW_BUILD_DOCS=OFF -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -S ../ -B .
make && make Shaders && ./BrainEngine
cd ..

#!/bin/sh

#compile all shader files
/Users/c2/VulkanSDK/1.3.283.0/macOS/bin/glslc shaders/*.vert -o shaders/*.vert.spv
/Users/c2/VulkanSDK/1.3.283.0/macOS/bin/glslc shaders/*.frag -o shaders/*.frag.spv

#build cmake file
cd out/build ; make
#!/bin/bash

rm lib/*.o
gcc -Iinclude -c vendor/glad.c -o lib/glad.o
gcc -Iinclude -c vendor/imgui/*.cpp vendor/imgui/backends/imgui_impl_glfw.cpp vendor/imgui/backends/imgui_impl_opengl3.cpp
mv *.o lib
rm -r vendor/glfw-build
mkdir vendor/glfw-build
cmake -S vendor/glfw -B vendor/glfw-build
cd vendor/glfw-build
make
cp src/libglfw3.a ../../lib/libglfw3.a
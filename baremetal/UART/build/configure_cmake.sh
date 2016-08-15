#!/bin/sh

cmake -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=./toolchain-arm-none-eabi-rpi2.cmake ../source
# Generator is for Unix not e.g. Borland Makefiles or Visual Studio
# -DCMAKE_TOOLCHAIN_FILE= cmake script which sets up all the toolchain related variables
# path to source

# https://cmake.org/cmake/help/v2.8.12/cmake.html#opt:-Ggenerator-name
# http://www.vtk.org/Wiki/CMake_Cross_Compiling

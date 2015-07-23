cmake_minimum_required(VERSION 3.2)
project (imgui)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

add_library(lodepng STATIC lodepng.cpp)

set_property(TARGET lodepng PROPERTY CXX_STANDARD 14)
set_property(TARGET lodepng PROPERTY CXX_STANDARD_REQUIRED 14)

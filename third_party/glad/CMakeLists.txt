cmake_minimum_required(VERSION 3.2)
project (imgui)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")

add_library(glad_loader STATIC glad.c)

set_property(TARGET glad_loader PROPERTY CXX_STANDARD 14)
set_property(TARGET glad_loader PROPERTY CXX_STANDARD_REQUIRED 14)

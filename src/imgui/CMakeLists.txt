cmake_minimum_required(VERSION 3.2)
project (imgui)

include_directories(${CMAKE_SOURCE_DIR}/include)

set(IMGUI_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

add_library(imgui STATIC imgui.cpp imgui_impl_glfw_gl3.cpp)
target_link_libraries(imgui ${GLFW_LIBRARIES})

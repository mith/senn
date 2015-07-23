cmake_minimum_required(VERSION 3.2)
project (imgui)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

add_library(imgui STATIC imgui.cpp imgui_impl_glfw_gl3.cpp)

set_property(TARGET imgui PROPERTY CXX_STANDARD 14)
set_property(TARGET imgui PROPERTY CXX_STANDARD_REQUIRED 14)

target_link_libraries(imgui ${GLFW_LIBRARIES} ${GLEW_LIBRARIES})

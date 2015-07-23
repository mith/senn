cmake_minimum_required(VERSION 3.2)
project (senn)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Wformat=2") 
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcolor-diagnostics")
endif()
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto") 

set(CMAKE_EXPORT_COMPILE_COMMANDS "ON")

find_package(glbinding REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(Boost REQUIRED COMPONENTS iostreams)
pkg_search_module(YAML REQUIRED yaml-cpp)
pkg_search_module(GLFW REQUIRED glfw3)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)

include_directories("./include" 
                    ${GLBINDING_INCLUDE_DIR}
                    ${GLFW_INCLUDE_DIR}
                    ${YAML_INCLUDE_DIR})

add_subdirectory("src/imgui" "imgui")
include_directories(${IMGUI_INCLUDE_DIR})

add_subdirectory("src/renderer" "renderer")
add_subdirectory("textures" "textures")

set(senn_sources src/main.cpp src/recompiler.cpp)
add_executable(senn ${senn_sources})

set_property(TARGET senn PROPERTY CXX_STANDARD 14)
set_property(TARGET senn PROPERTY CXX_STANDARD_REQUIRED 14)

target_link_libraries(senn dl 
                           pthread 
                           ${GLFW_LIBRARIES}
                           ${YAML_LIBRARIES}
                           ${GLBINDING_LIBRARIES}
                           ${OPENGL_LIBRARIES}
                           ${GLEW_LIBRARIES}
                           imgui)

add_dependencies(senn textures)


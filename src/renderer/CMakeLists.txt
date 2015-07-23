cmake_minimum_required(VERSION 3.2)
project (renderer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Wformat=2") 
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcolor-diagnostics")
endif()
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto") 

set(renderer_sources renderer.cpp utils.cpp 
    gl_utils.cpp meshloader.cpp 
    scene.cpp shaderloader.cpp)

add_library(renderer SHARED ${renderer_sources})

set_property(TARGET renderer PROPERTY CXX_STANDARD 14)
set_property(TARGET renderer PROPERTY CXX_STANDARD_REQUIRED 14)

include_directories(${GLFW_INCLUDE_DIR} 
                    ${IMGUI_INCLUDE_DIR}
                    ${BOOST_INCLUDE_DIR}
                    ${GLM_INCLUDE_DIR}
                    ${YAML_INCLUDE_DIR})

target_link_libraries(renderer ${GLFW_LIBRARIES} 
                               ${GLBINDING_LIBRARIES} 
                               ${Boost_LIBRARIES}
                               ${GLEW_LIBRARIES}
                               ${YAML_LIBRARIES}
                               lodepng
                               ktx
                               imgui)

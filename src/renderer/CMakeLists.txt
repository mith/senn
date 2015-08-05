cmake_minimum_required(VERSION 3.2)
project (renderer)

set(renderer_sources renderer.cpp utils.cpp 
    gl_utils.cpp meshloader.cpp 
    scene.cpp shaderloader.cpp dirwatcher.cpp
    sceneloader.cpp primitives.cpp camera.cpp
    light.cpp)

add_library(renderer SHARED ${renderer_sources})

set_property(TARGET renderer PROPERTY CXX_STANDARD 14)
set_property(TARGET renderer PROPERTY CXX_STANDARD_REQUIRED 14)

include_directories(${GLFW_INCLUDE_DIR} 
                    ${IMGUI_INCLUDE_DIR}
                    ${BOOST_INCLUDE_DIR}
                    ${GLM_INCLUDE_DIR}
                    ${YAML_INCLUDE_DIR})

message(${OPENGL_LIBRARIES})

target_link_libraries(renderer ${GLFW_LIBRARIES} 
                               ${Boost_LIBRARIES}
                               ${YAML_LIBRARIES}
                               ${GLEW_LIBRARIES}
                               ${OPENGL_LIBRARIES}
                               lodepng
                               ktx
                               imgui)

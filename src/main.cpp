#include <iostream>
#include <thread>

#include "libreloader.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw_gl3.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>


void glfw_error(int error, const char* description)
{
    std::cerr << "glfw error: " << error << " : " << description << std::endl;
}

std::string glenum_to_string(GLenum e)
{
    switch(e){
        case GL_DEBUG_SOURCE_APPLICATION:
            return "GL_DEBUG_SOURCE_APPLICATION";
        case GL_DEBUG_SOURCE_API:
            return "GL_DEBUG_SOURCE_API";
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            return "GL_DEBUG_SOURCE_SHADER_COMPILER";
        case GL_DEBUG_TYPE_ERROR:
            return "GL_DEBUG_TYPE_ERROR";
        case GL_DEBUG_TYPE_PERFORMANCE:
            return "GL_DEBUG_TYPE_PERFORMANCE";
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            return "GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR";
        case GL_DEBUG_SEVERITY_LOW:
            return "GL_DEBUG_SEVERITY_LOW";
        case GL_DEBUG_SEVERITY_MEDIUM:
            return "GL_DEBUG_SEVERITY_MEDIUM";
        case GL_DEBUG_SEVERITY_HIGH:
            return "GL_DEBUG_SEVERITY_HIGH";
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            return "GL_DEBUG_SEVERITY_NOTIFICATION";
        default:
            return std::string("Add to glenum_to_string: ") + std::to_string(e);
    }
}

void gl_error(GLenum source, GLenum type, GLuint id, GLenum severity,
    GLsizei length, const GLchar* msg, const void* userParam)
{
    std::cerr.write(msg, length) 
        << " " << id
        << " " << glenum_to_string(source) 
        << " " << glenum_to_string(type)
        << " " << glenum_to_string(severity)
        << std::endl;
}

GLFWwindow* init_gl()
{
    glfwSetErrorCallback(glfw_error);
    if (!glfwInit()) {
        throw std::runtime_error("failure initializing glfw");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DEPTH_BITS, 32);
    glfwWindowHint(GLFW_STENCIL_BITS, 0);
    glfwWindowHint(GLFW_SRGB_CAPABLE, GL_TRUE);

    int num_monitors;
    GLFWmonitor** monitors = glfwGetMonitors(&num_monitors);
    GLFWwindow* window = glfwCreateWindow(1680, 1050, "senn", monitors[1], nullptr);

    if (window == nullptr) {
        glfwTerminate();
        throw std::runtime_error("failure opening window");
    }

    glfwMakeContextCurrent(window);
    auto glew_err = glewInit();
    if (glew_err != GLEW_OK) {
        throw std::runtime_error((const char*)glewGetErrorString(glew_err));
    }

    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(gl_error, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE,
        0, nullptr, GL_TRUE);
    glDebugMessageControl(GL_DEBUG_SOURCE_APPLICATION,
        GL_DEBUG_TYPE_PUSH_GROUP, 
        GL_DONT_CARE, 
        0, nullptr, GL_FALSE);
    glDebugMessageControl(GL_DEBUG_SOURCE_APPLICATION,
        GL_DEBUG_TYPE_POP_GROUP, 
        GL_DONT_CARE, 
        0, nullptr, GL_FALSE);
    glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, 1,
        GL_DEBUG_SEVERITY_NOTIFICATION, -1, "gl callback test");

    glfwSwapInterval(1);
    return window;
}



int main()
{
    LibReloader rc("../src/renderer", "renderer", "./renderer", "ninja");
    LibFunctions lib_fun;
    auto window = init_gl();
    rc.refresh_lib(lib_fun, nullptr);
    auto rs = lib_fun.init(window);

    while (!glfwWindowShouldClose(window)) {
        if (rc.refresh_lib(lib_fun,rs)) {
            lib_fun.resume(rs);
        }
        auto do_reinit = lib_fun.tick(rs);
        if (do_reinit) {
            std::cout << "reinitializing" << std::endl;
            lib_fun.finish(rs);
            rs = lib_fun.init(window);
        }
    }
    ImGui::Shutdown();
    std::cout << "fin" << std::endl;
}

#include <iostream>
#include <chrono>
#include <thread>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw_gl3.hpp>

#include "recompiler.hpp"
#include "renderer/renderer.hpp"

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

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    std::cout << "key pressed: " << key << std::endl;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    ImGui_ImplGlfwGL3_KeyCallback(window, key, scancode, action, mods);
}

void window_resize_callback(GLFWwindow*, int width, int height)
{
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
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
    GLFWwindow* window = glfwCreateWindow(512, 512, "senn", nullptr, nullptr);

    if (window == nullptr) {
        glfwTerminate();
        throw std::runtime_error("failure opening window");
    }

    glfwMakeContextCurrent(window);
    if (glewInit() != GLEW_OK) {
        throw std::runtime_error("failure loading opengl functions");
    }

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
        GL_DEBUG_SEVERITY_NOTIFICATION, -1, "gl callback test\n\0");

    glfwSwapInterval(1);
    return window;
}

int main()
{
    auto window = init_gl();
    Recompiler rc("renderer");
    LibFunctions lib_fun;
    rc.refresh_lib(lib_fun);
    auto rs = lib_fun.init(window);
    ImGui_ImplGlfwGL3_Init(rs->window, true);
    glfwSetKeyCallback(rs->window, key_callback);
    glfwSetWindowSizeCallback(rs->window, window_resize_callback);
    window_resize_callback(rs->window, 512, 512);
    while (!glfwWindowShouldClose(rs->window)) {
        ImGui_ImplGlfwGL3_NewFrame();
        glfwPollEvents();
        if (rc.refresh_lib(lib_fun)) {
            lib_fun.update(rs);
        }
        lib_fun.tick(rs);
        ImGui::Render();
        glfwSwapBuffers(rs->window);
    }
    std::cout << "fin" << std::endl;
}

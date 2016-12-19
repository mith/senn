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

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(512, 512, "senn", nullptr, nullptr);

    if (window == nullptr) {
        glfwTerminate();
        throw std::runtime_error("failure opening window");
    }

    glfwMakeContextCurrent(window);
    auto glew_err = glewInit();
    if (glew_err != GLEW_OK) {
        throw std::runtime_error((const char*)glewGetErrorString(glew_err));
    }

    glfwMakeContextCurrent(window);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    return window;
}

bool gl_extension_check()
{
  if(!GLEW_ARB_debug_output) {
    std::cerr << "ARB_debug_output not available" << std::endl;
  }

  if (!GLEW_ARB_multi_draw_indirect) {
    std::cerr << "ARB_multi_draw_indirect not available" << std::endl;
  }

  if (!GLEW_ARB_direct_state_access) {
    std::cerr << "ARB_direct_state_access not available" << std::endl;
  }
}

void set_callbacks(GLFWwindow* window) 
{
    glfwSetErrorCallback(glfw_error);

    glfwSetKeyCallback(window, key_callback);
    glfwSetWindowSizeCallback(window, window_resize_callback);

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
}


int main()
{
    LibReloader rc("../src/renderer", "renderer", "./renderer", "ninja");
    LibFunctions lib_fun;
    auto window = init_gl();
    if (!gl_extension_check()) {
      std::exit(EXIT_FAILURE);
    }
    set_callbacks(window);
    window_resize_callback(window, 512, 512);
    rc.refresh_lib(lib_fun, nullptr);
    auto rs = lib_fun.init(window);
    while (true) {
        if (rc.refresh_lib(lib_fun,rs)) {
            lib_fun.resume(rs);
        }
        lib_fun.tick(rs);
    }
    std::cout << "fin" << std::endl;
}

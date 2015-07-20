#include <iostream>
#include <chrono>
#include <thread>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw_gl3.hpp>

#include "recompiler.hpp"
#include "renderer/renderer.hpp"
#include <gli/gli.hpp>

void glfw_error(int error, const char* description)
{
    std::cerr << "glfw error: " << error << " : " << description << std::endl;
}

void gl_error(GLenum, GLenum, GLuint, GLenum,
              GLsizei length, const GLchar* msg, const void*)
{
    std::cerr.write(msg, length);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    std::cout << "key pressed: " << key << std::endl;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    ImGui_ImplGlfwGL3_KeyCallback(window, key, scancode, action, mods);
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
    GLFWwindow* window = glfwCreateWindow(800, 600, "senn", nullptr, nullptr);

    if (window == nullptr) {
        glfwTerminate();
        throw std::runtime_error("failure opening window");
    }

    glfwMakeContextCurrent(window);
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        throw std::runtime_error("failure loading opengl functions");
    }

    glDebugMessageCallback(gl_error, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE,
                          0, nullptr, GL_TRUE);
    glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, 1,
                         GL_DEBUG_SEVERITY_NOTIFICATION, -1, "gl callback test\n\0");
    
    glfwSwapInterval(1);
    return window;
}
int main()
{
    recompiler rc("renderer");
    lib_functions lib_fun;
    rc.refresh_lib(lib_fun);
    auto rs = lib_fun.init();
    rs->window = init_gl();
    ImGui_ImplGlfwGL3_Init(rs->window, true);
    glfwSetKeyCallback(rs->window, key_callback);
    while (!glfwWindowShouldClose(rs->window)) {
        ImGui_ImplGlfwGL3_NewFrame();
        glfwPollEvents();
        if (rc.refresh_lib(lib_fun)) {
            lib_fun.update(rs);
        }
        lib_fun.tick(rs);
        //std::cout << '\r'<< glfwGetTime();
        //std::cout.flush();
        glClearColor(0.2f, 0.3f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui::Render();
        glfwSwapBuffers(rs->window);
    }
    std::cout << "fin" << std::endl;
}

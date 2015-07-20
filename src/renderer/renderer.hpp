#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

struct renderer_state {
    GLFWwindow* window;
};

extern "C" {
    renderer_state* init();
    void update(renderer_state*);
    void tick(renderer_state*);
}

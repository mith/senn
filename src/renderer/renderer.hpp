#pragma once

#include <array>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "shader_loader.hpp"
#include "scene.hpp"

struct renderer_state {
    GLFWwindow* window;
    scene loaded_scene;
    shader_loader sh_loader;
};

extern "C" {
renderer_state* init(GLFWwindow*);
void update(renderer_state*);
void tick(renderer_state*);
}

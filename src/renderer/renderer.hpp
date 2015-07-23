#pragma once

#include <array>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <memory>

#include "shaderloader.hpp"
#include "scene.hpp"

struct RendererState {
    GLFWwindow* window;
    std::unique_ptr<Scene> loaded_scene;
    std::unique_ptr<ShaderLoader> shader_loader;
    RendererState()
    {
        shader_loader = std::make_unique<ShaderLoader>("../shaders");
    }
};

extern "C" {
RendererState* init(GLFWwindow*);
void update(RendererState*);
void tick(RendererState*);
}

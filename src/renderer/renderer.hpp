#pragma once

#include <array>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <memory>

#include "shaderloader.hpp"
#include "sceneloader.hpp"

struct RendererState {
    GLFWwindow* window;
    Scene* loaded_scene;
    std::unique_ptr<SceneLoader> scene_loader;
    std::unique_ptr<ShaderLoader> shader_loader;
    std::unique_ptr<MeshLoader> mesh_loader;
    RendererState()
    {
        mesh_loader = std::make_unique<MeshLoader>("../meshes");
        shader_loader = std::make_unique<ShaderLoader>("../shaders");
        scene_loader = std::make_unique<SceneLoader>("../scenes", shader_loader.get(), mesh_loader.get());
    }
};

extern "C" {
RendererState* init(GLFWwindow*);
void suspend(RendererState*);
void resume(RendererState*);
void tick(RendererState*);
}

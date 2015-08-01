#include "renderer.hpp"
#include <iostream>
#include <cstdlib>
#include <array>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw_gl3.hpp>

#include "gl_utils.hpp"
#include "utils.hpp"
#include "scene.hpp"

#include <GL/glew.h>

RendererState* init(GLFWwindow* window)
{
    auto  state = new RendererState();
    state->window = window;
    state->loaded_scene = state->scene_loader->load_scene("cornell");
    return state;
}

void suspend(RendererState* state)
{
    state->shader_loader->stop();
    state->mesh_loader->stop();
    state->scene_loader->stop();
}

void resume(RendererState* state)
{
    glfwMakeContextCurrent(state->window);
    state->shader_loader->start();
    state->mesh_loader->start();
    state->scene_loader->start();
    ImGui::GetStyle().WindowRounding = 0.0f;
}

void tick(RendererState* state)
{
    state->shader_loader->update_shaders();
    state->scene_loader->update_scenes();
    state->mesh_loader->update_meshes();

    glDisable(GL_DEPTH_TEST);
    glClearColor(0.4f, 0.4f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    state->loaded_scene->render();

    ImGuiIO& imgio = ImGui::GetIO();
    ImGui::Value("FPS", imgio.Framerate);
    ImGui::Value("Frametime(ms)", imgio.DeltaTime * 1000);

    ImGui::Begin("Positions");
    for (auto & obj : state->loaded_scene->objects_attributes) {
        ImGui::Value("x", obj.position.x);
        ImGui::Value("z", obj.position.z);
    }
    ImGui::End();
}

#include "renderer.hpp"
#include <iostream>
#include <cstdlib>
#include <array>

#include <imgui.h>
#include <imgui_impl_glfw_gl3.hpp>

#define KTX_OPENGL 1
#include <ktx.h>
#include "gl_utils.hpp"
#include "utils.hpp"
#include "lodepng.h"
#include "scene.hpp"

RendererState* init(GLFWwindow* window)
{
    auto state = new RendererState();
    state->window = window;
    state->loaded_scene = std::make_unique<Scene>(Scene::load_from_file("../scenes/cornell.yaml", state->shader_loader.get()));
    return state;
}

void update(RendererState* state)
{
    glfwMakeContextCurrent(state->window);
    ImGui::GetStyle().WindowRounding = 0.0f;
}

void tick(RendererState* state)
{

    glDisable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    state->loaded_scene->render();

    ImGuiIO& imgio = ImGui::GetIO();
    ImGui::Value("FPS", imgio.Framerate);
    ImGui::Value("Frametime(ms)", imgio.DeltaTime * 1000);

    ImGui::Begin("Positions");

    for (size_t i = 0; i < state->loaded_scene->objects_attributes.size(); i++) {
        auto& objects_attributes = state->loaded_scene->objects_attributes[i];
        ImGui::Value(std::to_string(i).c_str(), objects_attributes.position.z);
    }
    ImGui::End();
}

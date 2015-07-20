#include "renderer.hpp"
#include <iostream>
#include <cstdlib>

#include <imgui.h>
#include <imgui_impl_glfw_gl3.hpp>

renderer_state* init()
{
    auto state = new renderer_state();
    return state;
}

void update(renderer_state* state)
{
    glfwMakeContextCurrent(state->window);
    ImGui::GetStyle().WindowRounding = 0.0f;
}

void tick(renderer_state* state)
{
    glClearColor(0.3f, 0.4f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGuiIO & imgio = ImGui::GetIO();
    ImGui::Value("FPS", imgio.Framerate);
    ImGui::Value("Frametime(ms)", imgio.DeltaTime * 1000);
    //glfwSwapBuffers(state->window);
}

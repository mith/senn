#include "renderer.hpp"
#include <iostream>
#include <cstdlib>
#include <array>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw_gl3.hpp>

#include "gl_utils.hpp"
#include "utils.hpp"
#include "scene.hpp"

bool do_reinit;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (key == GLFW_KEY_F1 && action == GLFW_PRESS) {
        do_reinit = true;
    }

    ImGui_ImplGlfwGL3_KeyCallback(window, key, scancode, action, mods);
}

void window_resize_callback(GLFWwindow*, int width, int height)
{
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
}

void init_imgui(GLFWwindow* window)
{
    ImGui_ImplGlfwGL3_Init(window, true);
    auto& igs = ImGui::GetStyle();
    igs.WindowRounding = 2.0f;
    igs.FrameRounding = 2.0f;
    igs.GrabRounding = 2.0f;
    igs.ScrollbarRounding = 2.0f;
    igs.Colors[ImGuiCol_TitleBg] = ImVec4(0.39f, 0.39f, 0.39f, 0.45f);
    igs.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.20f, 0.20f, 0.20f, 0.20f);
    igs.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.59f, 0.59f, 0.59f, 0.55f);


}

void set_callbacks(GLFWwindow* window) 
{
    glfwSetKeyCallback(window, key_callback);
    glfwSetWindowSizeCallback(window, window_resize_callback);
}

RendererState* init(GLFWwindow* window)
{
    auto state = new RendererState();
    state->window = window;
    state->loaded_scene = state->scene_loader->load_scene("cornell");
    init_imgui(state->window);
    set_callbacks(state->window);
    window_resize_callback(window, 512, 512);
    return state;
}

void suspend(RendererState* state)
{
    state->shader_loader->stop();
    state->mesh_loader->stop();
    state->scene_loader->stop();
    ImGui_ImplGlfwGL3_Shutdown();
}

void resume(RendererState* state)
{
    glfwMakeContextCurrent(state->window);
    state->shader_loader->start();
    state->mesh_loader->start();
    state->scene_loader->start();
    init_imgui(state->window);
    set_callbacks(state->window);
}

namespace ImGui {
    void Value(const char* prefix, glm::vec3 v) 
    {
        std::array<char, 200> cb;
        std::snprintf(cb.data(), cb.size(), "%s: %.3f, %.3f, %.3f", prefix, v.x, v.y, v.z);
        ImGui::Text(cb.data());
    }
}

bool tick(RendererState* state)
{
    state->shader_loader->update_shaders();
    state->scene_loader->update_scenes();
    state->mesh_loader->update_meshes();
    glfwPollEvents();
    state->loaded_scene->update(state->window);
    ImGui_ImplGlfwGL3_NewFrame();
    state->loaded_scene->render();

    ImGuiIO& imgio = ImGui::GetIO();
    ImGui::Begin("Debug");
    ImGui::Value("FPS", imgio.Framerate);
    ImGui::Value("Frametime(ms)", imgio.DeltaTime * 1000);

    if (ImGui::TreeNode("Camera")) {
        ImGui::Value("yaw", state->loaded_scene->camera.yaw);
        ImGui::Value("pitch", state->loaded_scene->camera.pitch);
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Objects")) {
        for (auto& obj : state->loaded_scene->objects_attributes) {
            ImGui::Value("position", obj.position);
        }
        ImGui::TreePop();
    }
    ImGui::SetWindowSize(ImVec2(0, 0));
    ImGui::End();

    ImGui::Render();
    glfwSwapBuffers(state->window);

    auto dr = do_reinit;
    do_reinit = false;
    return dr;
}

void finish(RendererState* state)
{
    delete state;
}

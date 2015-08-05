#include "scene.hpp"

#include <iostream>

#include <yaml-cpp/yaml.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "meshloader.hpp"
#include "gl_utils.hpp"
#include "utils.hpp"
#include "shaderloader.hpp"

Scene::Scene()
{
    last_cursor_pos = {0.0, 0.0};
}

Scene::~Scene()
{
}

void Scene::render_shadowmap()
{    
    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 2, -1, "rendering shadowmap");
    for (auto & light : directional_lights) {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, light.shadowmap.framebuffer.get_name());
     
        glClearDepth(1.0f);
        glClear(GL_DEPTH_BUFFER_BIT);

        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(2.0f, 2.0f);

        glViewport(0, 0, light.shadowmap.size.x, light.shadowmap.size.y);

        glUseProgram(light.shadowmap.shader->name);

        auto model_mat_attrib = glGetUniformLocation(light.shadowmap.shader->name, "modelMat");
        auto shadow_mat_attrib = glGetUniformLocation(light.shadowmap.shader->name, "perspMat");


        glUniformMatrix4fv(shadow_mat_attrib, 1, GL_FALSE, 
                glm::value_ptr(light.shadowmap.projection_matrix()));

        for (int d = 0; d < (int)meshes.size(); d++) {
            auto& mesh = meshes[d];
            auto& obj_atr = objects_attributes[d];

            glBindVertexArray(mesh->vao.name);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indice_buffer.name);

            auto rotation_matrix = glm::mat4_cast(obj_atr.orientation);
            
            glm::mat4 model_mat = glm::translate(obj_atr.position)
                                * rotation_matrix
                                * glm::scale(obj_atr.scale);
            glUniformMatrix4fv(model_mat_attrib, 1, GL_FALSE, glm::value_ptr(model_mat));

            glDrawElementsBaseVertex(GL_TRIANGLES, mesh->draw_command.index_count * 3,
                GL_UNSIGNED_INT, (void*)(mesh->draw_command.base_index * sizeof(unsigned int)),
                mesh->draw_command.base_vertex);
        }
        glBindVertexArray(0);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glDisable(GL_POLYGON_OFFSET_FILL);
        glGenerateTextureMipmap(light.shadowmap.depth_texture.name);
    }
    glPopDebugGroup();
}

void Scene::update(GLFWwindow* window)
{
    float x = std::fmod(glfwGetTime(), 2 * M_PI) - M_PI;
    objects_attributes[1].position.z = std::sin(x) * 15.0f;
    objects_attributes[1].position.y = std::sin(x) * 10.0f;
    objects_attributes[1].position.x = std::cos(x) * 15.0f;


    if (glfwGetWindowAttrib(window, GLFW_FOCUSED)) {
    
        glm::vec3 d;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            d.z = 1.0f;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            d.z = -1.0f;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            d.x = 1.0f;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            d.x = -1.0f;
        }

        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            d *= 10;
        }

        camera.move_rotated(d);

        glm::vec3 e;
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            e.y = 1.0f;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL)) {
            e.y = -1.0f;
        }
        camera.move(e);

        glfwGetWindowSize(window, &camera.size.x, &camera.size.y);

        glm::dvec2 cur_cursor_pos;
        glfwGetCursorPos(window, &cur_cursor_pos[0], &cur_cursor_pos[1]);

        camera.add_yaw(-(cur_cursor_pos.x - last_cursor_pos.x) * 0.3);
        camera.add_pitch((cur_cursor_pos.y - last_cursor_pos.y) * 0.3);
        last_cursor_pos = cur_cursor_pos;
    }
}

void Scene::render()
{
    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 2, -1, "rendering Scene");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CW);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    glDepthRangedNV(-1.0f, 1.0f);

    render_shadowmap();

    glViewport(0, 0, camera.size.x, camera.size.y);
    glCullFace(GL_BACK);

    glClearDepth(1.0f);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glUseProgram(shader->name);

    auto model_mat_attrib = glGetUniformLocation(shader->name, "modelMat");
    auto rot_mat_attrib = glGetUniformLocation(shader->name, "rotMat");
    auto sampler_attrib = glGetUniformLocation(shader->name, "diffuse");
    auto shadowmap_attrib = glGetUniformLocation(shader->name, "shadowmap");
    auto dir_light_attrib = glGetUniformLocation(shader->name, "directional_light");

    auto pers_mat_attrib = glGetUniformLocation(shader->name, "perspMat");
    auto shadow_mat_attrib = glGetUniformLocation(shader->name, "shadowMat");

    auto pers_mat = camera.get_projection_matrix()
                  * camera.get_view_matrix();
    auto shadow_offset = glm::mat4({0.5f, 0.0f, 0.0f, 0.0f},
                           {0.0f, 0.5f, 0.0f, 0.0f},
                           {0.0f, 0.0f, 0.5f, 0.0f},
                           {0.5f, 0.5f, 0.5f, 1.0f});

    auto & light = directional_lights[0];

    glBindTextureUnit(1, light.shadowmap.depth_texture.name);
    glUniform1i(shadowmap_attrib, 1);

    glUniformMatrix4fv(pers_mat_attrib, 1, GL_FALSE, glm::value_ptr(pers_mat));
    glUniformMatrix4fv(shadow_mat_attrib, 1, GL_FALSE, 
            glm::value_ptr(shadow_offset * light.shadowmap.projection_matrix()));
    glUniform3fv(dir_light_attrib, 1, glm::value_ptr(light.get_direction()));

    for (int d = 0; d < (int)meshes.size(); d++) {
        auto& mesh = meshes[d];
        auto& obj_atr = objects_attributes[d];

        glBindVertexArray(mesh->vao.name);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indice_buffer.name);

        auto rotation_matrix = glm::mat4_cast(obj_atr.orientation);
        
        glm::mat4 model_mat = glm::translate(obj_atr.position)
                            * rotation_matrix
                            * glm::scale(obj_atr.scale);
        glUniformMatrix4fv(model_mat_attrib, 1, GL_FALSE, glm::value_ptr(model_mat));
        glUniformMatrix4fv(rot_mat_attrib, 1, GL_FALSE, glm::value_ptr(rotation_matrix));

        glBindTextureUnit(0, mesh_textures[d]->name);
        glUniform1i(sampler_attrib, 0);

        glDrawElementsBaseVertex(GL_TRIANGLES, mesh->draw_command.index_count * 3,
            GL_UNSIGNED_INT, (void*)(mesh->draw_command.base_index * sizeof(unsigned int)),
            mesh->draw_command.base_vertex);
    }
    glBindVertexArray(0);
    glPopDebugGroup();
}

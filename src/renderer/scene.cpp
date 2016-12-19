#include "scene.hpp"

#include <iostream>

#include <yaml-cpp/yaml.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
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
        auto render_sm = [&](Shadowmap & shadowmap) {
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, shadowmap.framebuffer.get_name());
         
            glClipControl(GL_LOWER_LEFT, GL_NEGATIVE_ONE_TO_ONE);
            glDepthFunc(GL_LESS);
            glClearDepth(1.0f);
            glClear(GL_DEPTH_BUFFER_BIT);

            glCullFace(GL_BACK);
            glEnable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(2.0f, 2.0f);

            glViewport(0, 0, shadowmap.size.x, shadowmap.size.y);

            glUseProgram(shadowmap.shader->name);

            auto proj_mat_attrib = glGetUniformLocation(shadowmap.shader->name, "projMat");

            for (int d = 0; d < (int)meshes.size(); d++) {
                auto& mesh = meshes[d];

                glBindVertexArray(mesh->vao.name);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indice_buffer.get_name());

                auto proj_mat = shadowmap.projection_matrix()
                              * model_matrix(objects_attributes[d]);

                glUniformMatrix4fv(proj_mat_attrib, 1, GL_FALSE, glm::value_ptr(proj_mat));

                glDrawElementsBaseVertex(GL_TRIANGLES, mesh->draw_command.index_count * 3,
                    GL_UNSIGNED_INT, (void*)(mesh->draw_command.base_index * sizeof(unsigned int)),
                    mesh->draw_command.base_vertex);
            }
            glBindVertexArray(0);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            glDisable(GL_POLYGON_OFFSET_FILL);
            glGenerateTextureMipmap(shadowmap.depth_texture.name);
        };
        render_sm(light.shadowmap.near);
        render_sm(light.shadowmap.far);
    }
    glPopDebugGroup();
}

void Scene::update(GLFWwindow* window)
{
    float x = std::fmod(glfwGetTime(), 2 * M_PI) - M_PI;
    objects_attributes[1].position.z = std::sin(x) * 03.0f;
    objects_attributes[1].position.y = 1.0 + std::sin(x) * 01.0f;
    objects_attributes[1].position.x = std::cos(x) * 03.0f;

    glm::vec3 light_dir = glm::normalize(glm::vec3(1.00001f, 0.8f, 0.68301f));

    directional_lights[0].set_direction(light_dir);
    glm::vec3 cam_offset_near = (camera.get_rot_matrix() * glm::vec4(0.0f, 0.0f, 5.0f, 1.0f)).xyz();
    glm::vec3 cam_up =  {0.0f, 1.0f, 0.0f}; //glm::normalize(cam_offset);
    directional_lights[0].shadowmap.near.set_direction(light_dir, camera.position + glm::round(cam_offset_near / 10.0f) * 10.0f, cam_up);

    glm::vec3 cam_offset_far = (camera.get_rot_matrix() * glm::vec4(0.0f, 0.0f, 60.0f, 1.0f)).xyz();
    directional_lights[0].shadowmap.far.set_direction(light_dir, camera.position + glm::round(cam_offset_far / 10.0f) * 10.0f, cam_up);


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

        glm::vec3 e;
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            e.y = 1.0f;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL)) {
            e.y = -1.0f;
        }

        if (glm::length(d) > 0.0f) {
            d = glm::normalize(d);
        }

        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            d *= 1;
            e *= 1;
        } else if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
            d *= 0.01;
            e *= 0.01;
        } else {
            d *= 0.1;
            e *= 0.1;
        }



        camera.move(e);
        camera.move_rotated(d);

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
    glDepthMask(GL_TRUE);

    render_shadowmap();

    glViewport(0, 0, camera.size.x, camera.size.y);
    glCullFace(GL_BACK);

    glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);
    glDepthFunc(GL_GREATER);
    glClearDepth(0.0f);
    glClearColor(0.01f, 0.01f, 0.01f, 1.0f);
    //glDepthRangedNV(-1.0f, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glEnable(GL_DEPTH_CLAMP);
    glEnable(GL_FRAMEBUFFER_SRGB);

    glUseProgram(shader->name);

    auto model_mat_attrib = glGetUniformLocation(shader->name, "modelMat");
    auto rot_mat_attrib = glGetUniformLocation(shader->name, "rotMat");
    auto sampler_attrib = glGetUniformLocation(shader->name, "diffuse");
    auto shadowmap_near_attrib = glGetUniformLocation(shader->name, "shadowmap_near");
    auto shadowmap_far_attrib = glGetUniformLocation(shader->name, "shadowmap_far");
    auto dir_light_attrib = glGetUniformLocation(shader->name, "directional_light");
    auto light_intensity_attrib = glGetUniformLocation(shader->name, "light_intensity");
    auto camera_position_attrib = glGetUniformLocation(shader->name, "camera_position");

    auto pers_mat_attrib = glGetUniformLocation(shader->name, "perspMat");
    auto shadow_mat_near_attrib = glGetUniformLocation(shader->name, "shadowMat_near");
    auto shadow_mat_far_attrib = glGetUniformLocation(shader->name, "shadowMat_far");

    auto pers_mat = camera.get_projection_matrix()
                  * camera.get_view_matrix();
    auto shadow_offset = glm::mat4({0.5f, 0.0f, 0.0f, 0.0f},
                           {0.0f, 0.5f, 0.0f, 0.0f},
                           {0.0f, 0.0f, 0.5f, 0.0f},
                           {0.5f, 0.5f, 0.5f, 1.0f});

    auto & light = directional_lights[0];

    glBindTextureUnit(1, light.shadowmap.near.depth_texture.name);
    glUniform1i(shadowmap_near_attrib, 1);
    glBindTextureUnit(2, light.shadowmap.far.depth_texture.name);
    glUniform1i(shadowmap_far_attrib, 2);

    glUniformMatrix4fv(pers_mat_attrib, 1, GL_FALSE, glm::value_ptr(pers_mat));
    glUniformMatrix4fv(shadow_mat_near_attrib, 1, GL_FALSE, 
            glm::value_ptr(shadow_offset * light.shadowmap.near.projection_matrix()));
    glUniformMatrix4fv(shadow_mat_far_attrib, 1, GL_FALSE, 
            glm::value_ptr(shadow_offset * light.shadowmap.far.projection_matrix()));
    glUniform3fv(dir_light_attrib, 1, glm::value_ptr(light.get_direction()));
    glUniform1f(light_intensity_attrib, light.intensity);

    glUniform3fv(camera_position_attrib, 1, glm::value_ptr(camera.position));

    for (int d = 0; d < (int)meshes.size(); d++) {
        auto& mesh = meshes[d];
        auto& obj_atr = objects_attributes[d];

        glBindVertexArray(mesh->vao.name);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indice_buffer.get_name());

        auto rotation_matrix = glm::mat4_cast(obj_atr.orientation);
        
        glm::mat4 model_mat = model_matrix(obj_atr);
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

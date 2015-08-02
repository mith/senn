#include "scene.hpp"

#include <iostream>

#include <yaml-cpp/yaml.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/io.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "meshloader.hpp"
#include "gl_utils.hpp"
#include "utils.hpp"
#include "shaderloader.hpp"

Scene::Scene()
{
    light_direction = glm::normalize(glm::vec3(1.0f, 1.0f, 0.5f));
    shadow_mat = glm::ortho(-35.0f, 55.0f, -35.0f, 55.0f, -100.0f, 100.0f)
               * glm::lookAt(light_direction,
                             glm::vec3(0.0f, 0.0f, 0.0f),
                             glm::vec3(0.0f, 1.0f, 0.0f));
}

Scene::~Scene()
{
}

void Scene::render_shadowmap()
{    
    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 2, -1, "rendering shadowmap");
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, shadowmap.get_name());
 
    glClearDepth(1.0f);
    glClear(GL_DEPTH_BUFFER_BIT);

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(2.0f, 2.0f);

    glUseProgram(shadowmap_shader->name);

    auto model_mat_attrib = glGetUniformLocation(shadowmap_shader->name, "modelMat");
    auto shadow_mat_attrib = glGetUniformLocation(shadowmap_shader->name, "perspMat");

    glUniformMatrix4fv(shadow_mat_attrib, 1, GL_FALSE, glm::value_ptr(shadow_mat));

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
    glGenerateTextureMipmap(shadowmap_depth_tex.name);
    glPopDebugGroup();
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

    float x = std::fmod(glfwGetTime(), 2 * M_PI) - M_PI;
    objects_attributes[1].position.z = std::sin(x) * 15.0f;
    objects_attributes[1].position.y = std::sin(x) * 10.0f;
    objects_attributes[1].position.x = std::cos(x) * 15.0f;

    render_shadowmap();

    glCullFace(GL_BACK);

    glClearDepth(1.0f);
    glClearColor(0.1f, 0.3f, 0.1f, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glUseProgram(shader->name);

    auto model_mat_attrib = glGetUniformLocation(shader->name, "modelMat");
    auto rot_mat_attrib = glGetUniformLocation(shader->name, "rotMat");
    auto sampler_attrib = glGetUniformLocation(shader->name, "diffuse");
    auto shadowmap_attrib = glGetUniformLocation(shader->name, "shadowmap");
    auto dir_light_attrib = glGetUniformLocation(shader->name, "directional_light");

    auto pers_mat_attrib = glGetUniformLocation(shader->name, "perspMat");
    auto shadow_mat_attrib = glGetUniformLocation(shader->name, "shadowMat");

    float camx = std::sin(x * 0.5f) * 80.0f;
    float camy = std::sin(x * 0.1f) * 80.0f;
    float camz = std::cos(x * 0.5f) * 80.0f;

    auto pers_mat = glm::perspective(glm::radians(80.0f), 1.0f, 0.5f, 500.0f)
        * glm::lookAt(glm::vec3(camx, camy, camz),
                      glm::vec3(0.0f, 0.0f, 0.0f),
                      glm::vec3(0.0f, 1.0f, 0.0f));
    auto shadow_offset = glm::mat4({0.5f, 0.0f, 0.0f, 0.0f},
                           {0.0f, 0.5f, 0.0f, 0.0f},
                           {0.0f, 0.0f, 0.5f, 0.0f},
                           {0.5f, 0.5f, 0.5f, 1.0f});

    glBindTextureUnit(1, this->shadowmap_depth_tex.name);
    glUniform1i(shadowmap_attrib, 1);

    glUniformMatrix4fv(pers_mat_attrib, 1, GL_FALSE, glm::value_ptr(pers_mat));
    glUniformMatrix4fv(shadow_mat_attrib, 1, GL_FALSE, glm::value_ptr(shadow_offset * shadow_mat));
    glUniform3fv(dir_light_attrib, 1, glm::value_ptr(light_direction));

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

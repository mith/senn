#include "scene.hpp"

#include <iostream>

#include <yaml-cpp/yaml.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>

#include <GLFW/glfw3.h>

#include "meshloader.hpp"
#include "gl_utils.hpp"
#include "utils.hpp"
#include "shaderloader.hpp"

Scene::~Scene()
{
}

void Scene::render()
{
    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 2, -1, "rendering Scene");
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CW);
    glDepthFunc(GL_LESS);
    glDepthRange(0.0f, 1.0f);
    glDepthMask(GL_TRUE);

    glClearDepth(1.0f);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glUseProgram(shader->name);

    auto orientation_attrib = glGetUniformLocation(shader->name, "orientation");
    auto translation_attrib = glGetUniformLocation(shader->name, "translation");
    auto scale_attrib = glGetUniformLocation(shader->name, "scale");
    auto sampler_attrib = glGetUniformLocation(shader->name, "diffuse");
    auto dir_light_attrib = glGetUniformLocation(shader->name, "directional_light");
    auto point_light_attrib = glGetUniformLocation(shader->name, "point_light");

    auto pers_mat_attrib = glGetUniformLocation(shader->name, "perspMat");
    auto pers_mat = glm::perspective(glm::radians(80.0f), 1.0f, 0.5f, 500.0f)
        * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f),
                        glm::vec3(0.0f, 0.0f, -1.0f),
                        glm::vec3(0.0f, 1.0f, 0.0f));
    
    float x = fmod((glfwGetTime() * 1), 2 * M_PI) - M_PI;

    glUniformMatrix4fv(pers_mat_attrib, 1, GL_FALSE, glm::value_ptr(pers_mat));
    glUniform3fv(dir_light_attrib, 1, glm::value_ptr(glm::vec3(0.7f, 0.1f, 0.1f)));

    glm::vec3 point_light(std::sin(x) * 10, std::cos(x) * 10, -20.0f);
    glUniform3fv(point_light_attrib, 1, glm::value_ptr(point_light));

    for (int d = 0; d < (int)meshes.size(); d++) {
        auto& mesh = meshes[d];
        auto& obj_atr = objects_attributes[d];

        glBindVertexArray(mesh->vao.name);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indice_buffer.name);

        //auto rotation_matrix = glm::mat4_cast(glm::angleAxis((float)std::sinh(x) * 0.2f,
        //            glm::vec3(0.0f, 1.0f, 0.0f)));
        //obj_atr.position.z = -130.0 - std::cos(x) * 35;
        //obj_atr.position.x = std::sinh((std::fabs(x))) * 10;

        auto rotation_matrix = glm::mat4_cast(obj_atr.orientation);
        glUniformMatrix4fv(orientation_attrib, 1, GL_FALSE, glm::value_ptr(rotation_matrix));
        glUniform3fv(translation_attrib, 1, glm::value_ptr(obj_atr.position));
        glUniform3fv(scale_attrib, 1, glm::value_ptr(obj_atr.scale));

        glBindTextureUnit(0, mesh_textures[d]->name);
        glUniform1i(sampler_attrib, 0);

        glDrawElementsBaseVertex(GL_TRIANGLES, mesh->draw_command.index_count * 3,
            GL_UNSIGNED_INT, (void*)(mesh->draw_command.base_index * sizeof(unsigned int)),
            mesh->draw_command.base_vertex);
    }
    glBindVertexArray(0);
    glPopDebugGroup();
}

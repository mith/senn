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

void Scene::setup_vertex_array()
{
    glCreateVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, Vertex_buffer.name);
    glEnableVertexArrayAttrib(vao, 0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, attribute_buffer.name);
    glEnableVertexArrayAttrib(vao, 1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttribes), nullptr);
    glEnableVertexArrayAttrib(vao, 2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexAttribes),
        (void*)offsetof(VertexAttribes, texcoord));
}

Scene Scene::load_from_file(const std::string& filename, ShaderLoader* shader_loader)
{
    auto Scene_file = YAML::LoadFile(filename);

    Scene scene;
    scene.shader_loader = shader_loader;

    std::vector<Vertex> vertices;
    std::vector<VertexAttribes> attributes;
    std::vector<unsigned int> indices;

    for (auto n : Scene_file["objects"]) {
        auto md = load_mesh(n["mesh"].as<std::string>());

        DrawCommand draw;
        draw.index_count = md.indices.size();
        draw.base_vertex = vertices.size();
        draw.base_index = indices.size();
        scene.draw_commands.push_back(draw);

        ObjectAttribes obj_atr;
        obj_atr.orientation = n["orientation"].as<glm::quat>();
        obj_atr.position = n["position"].as<glm::vec3>();
        obj_atr.scale = n["scale"].as<glm::vec3>();
        scene.objects_attributes.push_back(obj_atr);

        vertices.insert(vertices.end(),
            md.vertices.begin(),
            md.vertices.end());
        attributes.insert(attributes.end(),
            md.attributes.begin(),
            md.attributes.end());
        indices.insert(indices.end(),
            md.indices.begin(),
            md.indices.end());
    }

    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 1, -1, "creating object buffers\0");

    glCreateBuffers(1, &scene.Vertex_buffer.name);
    glObjectLabel(GL_BUFFER, scene.Vertex_buffer.name, -1, "vertice buffer\0");
    glNamedBufferStorage(scene.Vertex_buffer.name, sizeof(Vertex) * vertices.size(),
        vertices.data(), 0);

    glCreateBuffers(1, &scene.attribute_buffer.name);
    glObjectLabel(GL_BUFFER, scene.attribute_buffer.name, -1, "attributes buffer\0");
    glNamedBufferStorage(scene.attribute_buffer.name, sizeof(VertexAttribes) * attributes.size(),
        attributes.data(), 0);

    glCreateBuffers(1, &scene.indice_buffer.name);
    glObjectLabel(GL_BUFFER, scene.indice_buffer.name, -1, "indice buffer\0");
    glNamedBufferStorage(scene.indice_buffer.name, sizeof(unsigned int) * indices.size(),
        indices.data(), 0);

    scene.setup_vertex_array();

    glPopDebugGroup();

    scene.shader = shader_loader->load_shader(ShaderDescriptor{"simple.vert", "simple.frag"});

    return scene;
}

void Scene::render()
{
    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 2, -1, "rendering Scene");
    glBindVertexArray(vao);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CW);
    glDepthFunc(GL_LESS);
    glDepthRange(0.0f, 1.0f);
    glDepthMask(GL_TRUE);

    glClearDepth(1.0f);
    glClearColor(0.1f, 0.14f, 0.1f, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glUseProgram(shader.name);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indice_buffer.name);

    auto orientation_attrib = glGetUniformLocation(shader.name, "orientation");
    auto translation_attrib = glGetUniformLocation(shader.name, "translation");
    auto scale_attrib = glGetUniformLocation(shader.name, "scale");

    auto pers_mat_attrib = glGetUniformLocation(shader.name, "perspMat");
    auto pers_mat = glm::perspective(glm::radians(80.0f), 1.0f, 0.5f, 500.0f)
        * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f),
                        glm::vec3(0.0f, 0.0f, -1.0f),
                        glm::vec3(0.0f, 1.0f, 0.0f));

    glUniformMatrix4fv(pers_mat_attrib, 1, GL_FALSE, glm::value_ptr(pers_mat));

    float x = fmod((glfwGetTime() * 0.7), 2 * M_PI);

    for (int d = 0; d < (int)draw_commands.size(); d++) {
        auto& mesh = draw_commands[d];
        auto& obj_atr = objects_attributes[d];

        auto rotation_matrix = glm::mat4_cast(glm::angleAxis((float)std::sin(x - M_PI) * 6,
                    glm::vec3(0.0f, 1.0f, 0.0f)));
        obj_atr.position.z = -90.0 - std::cos(x - M_PI) * 5;

        //auto rotation_matrix = glm::mat4_cast(obj_atr.orientation);
        glUniformMatrix4fv(orientation_attrib, 1, GL_FALSE, glm::value_ptr(rotation_matrix));
        glUniform3fv(translation_attrib, 1, glm::value_ptr(obj_atr.position));
        glUniform3fv(scale_attrib, 1, glm::value_ptr(obj_atr.scale));

        glDrawElementsBaseVertex(GL_TRIANGLES, mesh.index_count,
            GL_UNSIGNED_INT, (void*)(mesh.base_index * sizeof(unsigned int)),
            mesh.base_vertex);
    }
    glPopDebugGroup();
}

namespace YAML {
template <>
struct convert<glm::vec3> {
    static bool decode(const Node& node, glm::vec3& v)
    {
        if (node.size() != 3) {
            return false;
        }

        v.x = node[0].as<float>();
        v.y = node[1].as<float>();
        v.z = node[2].as<float>();
        return true;
    }
};

template <>
struct convert<glm::quat> {
    static bool decode(const Node& node, glm::quat& q)
    {
        if (node.size() != 4) {
            return false;
        }

        q.x = node[0].as<float>();
        q.y = node[1].as<float>();
        q.z = node[2].as<float>();
        q.w = node[3].as<float>();
        return true;
    }
};
}

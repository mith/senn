#pragma once

#include <vector>
#include <string>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "primitives.hpp"
#include "shaderloader.hpp"

class Scene {
    ShaderLoader* shader_loader;
    void setup_Vertex_array();
    Scene();

public:
    Shader shader;
    GLuint vao;

    Buffer<Vertex> Vertex_buffer;
    Buffer<VertexAttribes> attribute_buffer;
    Buffer<unsigned int> indice_buffer;
    std::vector<DrawCommand> DrawCommands;
    std::vector<ObjectAttribes> objects_attributes;

    static Scene load_from_file(const std::string& filename, ShaderLoader*);

    void render();
};

#pragma once

#include <vector>
#include <string>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "primitives.hpp"

class scene {
    void setup_vertex_array();

public:
    GLuint shader;
    GLuint vao;

    buffer<vertex> vertex_buffer;
    buffer<vertex_attributes> attribute_buffer;
    buffer<unsigned int> indice_buffer;
    std::vector<draw_command> draw_commands;
    std::vector<object_attributes> objects_attributes;

    static scene load_from_file(const std::string& filename);

    void render();
};

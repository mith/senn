#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <GL/glew.h>

struct vertex {
    glm::vec3 position;
    vertex(glm::vec3 position)
        : position(position)
    {
    }
};

struct vertex_attributes {
    glm::vec3 normal;
    glm::vec2 texcoord;
    vertex_attributes(glm::vec3 normal, glm::vec2 texcoord)
        : normal(normal)
        , texcoord(texcoord)
    {
    }
};

struct object_attributes {
    glm::quat orientation;
    glm::vec3 position;
    glm::vec3 scale;
};

struct draw_command {
    GLint index_count;
    GLint base_vertex;
    GLint base_index;
};

template <typename T>
struct buffer {
    GLuint name;
};

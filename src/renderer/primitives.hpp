#pragma once

#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <GL/glew.h>

struct Vertex {
    glm::vec3 position;
    Vertex(glm::vec3 position)
        : position(position)
    {
    }
};

struct VertexAttribes {
    glm::vec3 normal;
    glm::vec2 texcoord;
    VertexAttribes(glm::vec3 normal, glm::vec2 texcoord)
        : normal(normal)
        , texcoord(texcoord)
    {
    }
};

struct ObjectAttribes {
    glm::quat orientation;
    glm::vec3 position;
    glm::vec3 scale;
};

glm::mat4 model_matrix(const ObjectAttribes& object_attributes);

struct DrawCommand {
    GLint index_count;
    GLint base_vertex;
    GLint base_index;
};

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

struct DrawCommand {
    GLint index_count;
    GLint base_vertex;
    GLint base_index;
};

template <typename T>
struct Buffer {
    GLuint name;

    Buffer()
        : name(0)
    {
    }
    ~Buffer()
    {
        if (name != 0) {
            glDeleteBuffers(1, &name);
        }
    }

    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;
    Buffer(Buffer&& o)
        : name(o.name)
    {
        o.name = 0;
    }
    Buffer& operator=(Buffer&& o)
    {
        this->~Buffer();
        name = o.name;
        o.name = 0;
        return *this;
    }
};

struct VertexArray {
    GLuint name;

    VertexArray()
        : name(0)
    {
    }
    ~VertexArray()
    {
        if (name != 0) {
            glDeleteVertexArrays(1, &name);
        }
    }

    VertexArray(const VertexArray&) = delete;
    VertexArray& operator=(const VertexArray&) = delete;
    VertexArray(VertexArray&& o)
        : name(o.name)
    {
        o.name = 0;
    }
    VertexArray& operator=(VertexArray&& o)
    {
        this->~VertexArray();
        name = o.name;
        o.name = 0;
        return *this;
    }
};

struct Texture {
    GLuint name;
    Texture()
        : name(0)
    {
    }
    ~Texture()
    {
        if (name != 0) {
            glDeleteTextures(1, &name);
        }
    }
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    Texture(Texture&& o)
        : name(o.name)
    {
        o.name = 0;
    }
    Texture& operator=(Texture&& o)
    {
        glDeleteTextures(1, &name);
        name = o.name;
        o.name = 0;
        return *this;
    }
};

struct Material {
    Texture diffuse;
    float fresnel0;
    float roughness;
};

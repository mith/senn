#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

enum class Target {
    Vertex = GL_ARRAY_BUFFER,
    Index = GL_ELEMENT_ARRAY_BUFFER,
    Uniform = GL_UNIFORM_BUFFER
};

template <Target target, typename T>
struct Buffer;

template <typename T>
using VertexBuffer = Buffer<Target::Vertex, T>;

template <typename T>
using IndexBuffer = Buffer<Target::Index, T>;

template <typename T>
using UniformBuffer = Buffer<Target::Uniform, T>;

template <typename T>
VertexBuffer<T> make_vertex_buffer(GLsizeiptr, const T*, GLbitfield);

template <typename T>
IndexBuffer<T> make_index_buffer(GLsizeiptr, const T*, GLbitfield);

template <typename T>
UniformBuffer<T> make_uniform_buffer(GLsizeiptr, const T*, GLbitfield);

template <Target target, typename T>
struct Buffer {
protected:
    GLuint name;
    Buffer(GLsizeiptr size, const T* data, GLbitfield flags)
    {
        glCreateBuffers(1, &name);
        glNamedBufferStorage(name, sizeof(T) * size, data, flags);
    }

public:
    GLuint get_name() { return name; }

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

    friend VertexBuffer<T> make_vertex_buffer<T>(GLsizeiptr, const T*, GLbitfield);
    friend IndexBuffer<T> make_index_buffer<T>(GLsizeiptr, const T*, GLbitfield);
    friend UniformBuffer<T> make_uniform_buffer<T>(GLsizeiptr, const T*, GLbitfield);
};

template <typename T>
VertexBuffer<T> make_vertex_buffer(GLsizeiptr size, const T* data, GLbitfield flags)
{
    return VertexBuffer<T>(size, data, flags);
}

template <typename T>
IndexBuffer<T> make_index_buffer(GLsizeiptr size, const T* data, GLbitfield flags)
{
    return IndexBuffer<T>(size, data, flags);
}

template <typename T>
UniformBuffer<T> make_uniform_buffer(GLsizeiptr size, const T* data, GLbitfield flags)
{
    return UniformBuffer<T>(size, data, flags);
}

template<Target target, typename T>
void set_object_label(Buffer<target, T>& buffer, std::string label) {
    glObjectLabel(GL_BUFFER, buffer.get_name(), -1, label.c_str());
}

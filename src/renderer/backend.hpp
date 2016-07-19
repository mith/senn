#pragma once

#include <experimental/optional>

#include <GL/glew.h>
#include <glm/glm.hpp>

struct VertexArray {
    GLuint name;

    VertexArray()
    {
        glCreateVertexArrays(1, &name);
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

enum class TextureType {
    T2D = GL_TEXTURE_2D
};

template <TextureType Type>
struct Texture {
    GLuint name;
    Texture()
    {
        glCreateTextures(static_cast<GLenum>(Type), 1, &name);
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
        this->~Texture();
        name = o.name;
        o.name = 0;
        return *this;
    }
};

class Framebuffer {
    GLuint name;
    Texture<TextureType::T2D> depth_attachment;

public:
    Framebuffer()
    {
        glCreateFramebuffers(1, &name);
    }
    ~Framebuffer()
    {
        if (name > 0) {
            glDeleteFramebuffers(1, &name);
        }
    }
    Framebuffer(const Framebuffer&) = delete;
    Framebuffer operator=(const Framebuffer&) = delete;
    Framebuffer(Framebuffer&& o)
        : name(o.name)
    {
        o.name = 0;
    }
    Framebuffer& operator=(Framebuffer&& o)
    {
        this->~Framebuffer();
        name = o.name;
        o.name = 0;
        return *this;
    }
    GLuint get_name()
    {
        return name;
    }
    void set_depth_attachment(const Texture<TextureType::T2D>& tex)
    {
        glNamedFramebufferTexture(name, GL_DEPTH_ATTACHMENT, tex.name, 0);
    }
};

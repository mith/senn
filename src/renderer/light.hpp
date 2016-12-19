#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "backend.hpp"
#include "shaderloader.hpp"

struct Shadowmap {
    Texture<TextureType::T2D> depth_texture;
    Framebuffer framebuffer;
    Shader* shader;

    glm::ivec2 size;
    glm::mat4 view_matrix;
    glm::mat4 proj_matrix;

    Shadowmap(ShaderLoader* shader_loader, glm::ivec2 size, glm::vec3 direction);
    void set_direction(glm::vec3 direction, glm::vec3 offset, glm::vec3 up);
    void set_extents(glm::vec2 x, glm::vec2 y, glm::vec2 z);
    glm::mat4 projection_matrix()
    {
        return proj_matrix * view_matrix;
    }
};

struct CascadingShadowmap {
    struct Cascade {
        float far_z;
        glm::mat4 projection_matrix;
    };

    glm::ivec2 size;

    std::vector<Cascade> cascades;
    std::vector<Shadowmap> shadowmaps;

    CascadingShadowmap(ShaderLoader* shader_loader, float far, int split_count, glm::ivec2 size)
        : size(size)
    {
        float split_size = far / 3;
        for (int i = 0; i < split_count; i++) {
            shadowmaps.emplace_back(shader_loader, size, glm::normalize(glm::vec3(0.001, 0.99, 0.001)));
            float split_near = i * split_size;
            float split_far = split_near + split_size;
            cascades.emplace_back(split_far, glm::mat4(1.0f));
        }
    }

    void set_frustum(glm::mat4 projection_matrix);
};

struct DirectionalLight {
private:
    glm::vec3 direction;

public:
    glm::vec3 color;
    float intensity;
    CascadingShadowmap shadowmap;

    DirectionalLight(ShaderLoader*, glm::vec3 direction);
    void set_direction(glm::vec3 direction);
    const glm::vec3& get_direction() { return direction; }
};

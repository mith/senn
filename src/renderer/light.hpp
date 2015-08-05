#pragma once

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
    void set_direction(glm::vec3 direction);
    glm::mat4 projection_matrix()
    {
        return proj_matrix * view_matrix;
    }
};

struct DirectionalLight {
private:
    glm::vec3 direction;

public:
    glm::vec3 color;
    float intensity;
    Shadowmap shadowmap;

    DirectionalLight(ShaderLoader*, glm::vec3 direction);
    void set_direction(glm::vec3 direction);
    const glm::vec3& get_direction() { return direction; }
};

#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "primitives.hpp"
#include "backend.hpp"
#include "shaderloader.hpp"
#include "meshloader.hpp"

struct Material {
    Texture<TextureType::T2D> diffuse;
    float fresnel0;
    float roughness;
};

class Scene {
public:
    Texture<TextureType::T2D> shadowmap_depth_tex;
    Framebuffer shadowmap;
    Shader* shadowmap_shader;

    glm::mat4 shadow_mat;    
    glm::vec3 light_direction;

    Shader* shader;
    std::vector<Mesh*> meshes;
    std::vector<Texture<TextureType::T2D>*> mesh_textures;
    std::vector<ObjectAttribes> objects_attributes;
    std::unordered_map<std::string, Material> materials;

    Scene();
    ~Scene();

    Scene(const Scene&) = delete;
    Scene(Scene&&) = default;
    Scene& operator=(Scene&&) = default;

    void render();
    void render_shadowmap();
};

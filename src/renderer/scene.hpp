#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define GLM_SWIZZLE
#include <glm/glm.hpp>

#include "primitives.hpp"
#include "backend.hpp"
#include "shaderloader.hpp"
#include "meshloader.hpp"
#include "camera.hpp"
#include "light.hpp"

struct Material {
    Texture<TextureType::T2D> diffuse;
    float fresnel0;
    float roughness;
};


class Scene {
public:
    std::vector<DirectionalLight> directional_lights;

    Shader* shader;
    std::vector<Mesh*> meshes;
    std::vector<Texture<TextureType::T2D>*> mesh_textures;
    std::vector<ObjectAttribes> objects_attributes;
    std::unordered_map<std::string, Material> materials;

    Camera camera;
    glm::dvec2 last_cursor_pos;

    Scene();
    ~Scene();

    Scene(const Scene&) = delete;
    Scene(Scene&&) = default;
    Scene& operator=(Scene&&) = default;

    void update(GLFWwindow* window);
    void render();
    void render_shadowmap();
};

#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "primitives.hpp"
#include "shaderloader.hpp"
#include "meshloader.hpp"

class Scene {
public:
    Shader* shader;

    std::vector<Mesh*> meshes;
    std::vector<Texture*> mesh_textures;
    std::vector<ObjectAttribes> objects_attributes;
    std::unordered_map<std::string, Material> materials;

    Scene() {}
    ~Scene();

    Scene(const Scene&) = delete;
    Scene(Scene&&) = default;
    Scene& operator=(Scene&&) = default;

    void render();
};

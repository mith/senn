#pragma once

#include <string>
#include <unordered_map>
#include <memory>

#include "dirwatcher.hpp"
#include "scene.hpp"

class SceneLoader {
    const std::string scene_dir;
    ShaderLoader* shader_loader;
    MeshLoader* mesh_loader;
    DirWatcher scene_dir_watcher;

    std::unordered_map<std::string, Scene> loaded_scenes;

    Scene load_from_file(const std::string& filename);
public:
    SceneLoader(const std::string& scene_dir, ShaderLoader*, MeshLoader*);
    ~SceneLoader() { stop(); };
    void update_scenes();
    void stop();
    void start();

    Scene* load_scene(const std::string& scene_name);
};

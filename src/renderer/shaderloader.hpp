#pragma once

#include <thread>
#include <string>
#include <atomic>
#include <future>
#include <unordered_map>

#include "primitives.hpp"

struct ShaderDescriptor {
    std::string vertex;
    std::string fragment;

    bool operator==(const ShaderDescriptor& other) const
    {
        return vertex == other.vertex && fragment == other.fragment;
    }
};

namespace std {
    template<>
    struct hash<ShaderDescriptor>
    {
        std::size_t operator()(const ShaderDescriptor& descriptor) const
        {
            using std::hash;
            using std::string;
            return hash<string>()(descriptor.vertex)
                ^ (hash<string>()(descriptor.fragment) << 1) >> 1;
        }
    };
}

struct Shader {
    GLuint name;
};

class ShaderLoader {
    const std::string shader_dir;

    std::atomic_flag shaders_up_to_date;
    std::atomic<bool> watcher_should_run;
    std::future<void> watcher_stopped;

    std::unordered_map<ShaderDescriptor, Shader> loaded_shaders;

    void watch_shader_dir();
    GLuint compile_shader(const ShaderDescriptor&);

public:
    ShaderLoader(const std::string& shader_dir);
    void update_shaders();
    void stop();
    void start();

    Shader load_shader(const ShaderDescriptor&);
};

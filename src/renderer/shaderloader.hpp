#pragma once

#include <string>
#include <unordered_map>

#include "dirwatcher.hpp"
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
template <>
struct hash<ShaderDescriptor> {
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
    Shader()
        : name(0)
    {
    }
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    Shader(Shader&& o)
        : name(o.name)
    {
        o.name = 0;
    }
    Shader& operator=(Shader&& o)
    {
        if (name != 0) {
            glDeleteShader(name);
        }
        name = o.name;
        o.name = 0;
        return *this;
    }
};

class ShaderLoader {
    const std::string shader_dir;
    DirWatcher shader_dir_watcher;

    std::unordered_map<ShaderDescriptor, Shader> loaded_shaders;

    GLuint compile_shader(const ShaderDescriptor&);

public:
    ShaderLoader(const std::string& shader_dir);
    ~ShaderLoader() { stop(); }
    void update_shaders();
    void stop();
    void start();

    Shader* load_shader(const ShaderDescriptor&);
};

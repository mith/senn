#include "shaderloader.hpp"

#include <string.h>
#include <iostream>

#include "utils.hpp"
#include "gl_utils.hpp"

std::ostream& operator<<(std::ostream& stream, const ShaderDescriptor& descriptor)
{
    std::cout << "[" << descriptor.vertex << ", " << descriptor.fragment << "]";
    return stream;
}

ShaderLoader::ShaderLoader(const std::string& shader_dir)
    : shader_dir(shader_dir)
    , shader_dir_watcher(shader_dir)
{
    start();
}

void ShaderLoader::update_shaders()
{
    if (shader_dir_watcher.dir_changed()) {

        for (auto& loaded_shader : loaded_shaders) {
            try { 
                auto& name = loaded_shader.second.name;
                name = compile_shader(loaded_shader.first);
                std::cout << "shader " << loaded_shader.first << " reloaded" << std::endl;
            } catch (std::runtime_error e) {
                std::cerr << e.what() << std::endl;
            }
        }
    }
}

Shader* ShaderLoader::load_shader(const ShaderDescriptor& descriptor)
{
    auto it = loaded_shaders.find(descriptor);
    if (it != loaded_shaders.end()) {
        return &it->second;
    }

    Shader shader;
    shader.name = compile_shader(descriptor);
    loaded_shaders.emplace(descriptor, std::move(shader));
    return &loaded_shaders.find(descriptor)->second;
}

GLuint ShaderLoader::compile_shader(const ShaderDescriptor& descriptor)
{
    auto vertex_shader_src = file_to_str(shader_dir + "/" + descriptor.vertex);
    auto frag_shader_src = file_to_str(shader_dir + "/" + descriptor.fragment);

    return create_program({ create_shader(GL_VERTEX_SHADER, vertex_shader_src),
            create_shader(GL_FRAGMENT_SHADER, frag_shader_src) });
}

void ShaderLoader::stop()
{
    shader_dir_watcher.stop();
}

void ShaderLoader::start()
{
    shader_dir_watcher.start();
}

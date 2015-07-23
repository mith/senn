#pragma once

#include <thread>
#include <string>
#include <atomic>
#include <future>
#include <map>

#include "primitives.hpp"

struct shader_descriptor {
    std::string vertex;
    std::string fragment;
};

struct shader {
    GLuint name;
};

class shader_loader {
    const std::string shader_dir;

    std::atomic_flag shaders_up_to_date;
    std::atomic<bool> watcher_should_run;
    std::future<void> watcher_stopped;

    std::map<const std::string, shader> loader_shaders;

    void watch_shader_dir();
    GLuint compile_shader(shader_descriptor);

public:
    shader_loader(const std::string& shader_dir);
    void update_shaders();
    void stop();
    void start();

    shader load_shader(const std::string&);
};

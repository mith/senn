#include "shaderloader.hpp"

#include <sys/inotify.h>
#include <stdexcept>
#include <unistd.h>
#include <poll.h>
#include <string.h>
#include <iostream>

#include "utils.hpp"
#include "gl_utils.hpp"

ShaderLoader::ShaderLoader(const std::string& shader_dir)
    : shader_dir(shader_dir)
    , shaders_up_to_date(ATOMIC_FLAG_INIT)
{
    start();
}

#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUF_LEN (1024 * (EVENT_SIZE + 16))

void ShaderLoader::watch_shader_dir()
{
    int fd = inotify_init();
    if (fd < 0) {
        throw std::runtime_error(std::string("error initializing inotify: ") + strerror(errno));
    }
    int wd = inotify_add_watch(fd, shader_dir.c_str(), IN_MODIFY | IN_CREATE | IN_DELETE);
    if (wd < 0) {
        throw std::runtime_error(std::string("error setting watch for dir ")
                                     + shader_dir + std::string(" ") + strerror(errno));
    }

    pollfd pfd = { fd, POLLIN, 0 };
    char buf[BUF_LEN];

    while (watcher_should_run) {
        int ret = poll(&pfd, 1, 500);
        if (ret < 0) {
            std::cerr << "shader poll failed: " << strerror(errno) << std::endl;
        }
        else if (ret > 0) {
            int len = read(fd, buf, BUF_LEN);
            int i = 0;
            while (len < 0) {
                inotify_event* event;
                event = (inotify_event*)&buf[i];
                shaders_up_to_date.clear();
                i += EVENT_SIZE + event->len;
            }
        }
    }

    inotify_rm_watch(fd, wd);
    close(fd);
}


void ShaderLoader::update_shaders()
{
    if (shaders_up_to_date.test_and_set()) {
        return;
    }

    for (auto& loaded_shader : loaded_shaders) {
        auto& name = loaded_shader.second.name;
        glDeleteProgram(name);
        name = compile_shader(loaded_shader.first);
    }
}

Shader ShaderLoader::load_shader(const ShaderDescriptor& descriptor)
{
    auto it = loaded_shaders.find(descriptor);
    if (it != loaded_shaders.end()) {
        return it->second;
    }

    Shader shader;
    shader.name = compile_shader(descriptor);
    loaded_shaders.insert(std::make_pair(descriptor, shader));
    return shader;
}

GLuint ShaderLoader::compile_shader(const ShaderDescriptor& descriptor)
{
    auto vertex_shader_src = file_to_str(descriptor.vertex);
    auto frag_shader_src = file_to_str(descriptor.fragment);

    return create_program({ create_shader(GL_VERTEX_SHADER, vertex_shader_src),
        create_shader(GL_FRAGMENT_SHADER, frag_shader_src) });
}

void ShaderLoader::stop()
{
    watcher_should_run = false;
    watcher_stopped.wait();
}

void ShaderLoader::start()
{
    watcher_stopped = std::async(std::launch::async, [&]() {watch_shader_dir(); });
}

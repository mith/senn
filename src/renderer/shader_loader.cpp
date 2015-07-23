#include "shader_loader.hpp"

#include <sys/inotify.h>
#include <stdexcept>
#include <unistd.h>

shader_loader::shader_loader(const std::string& shader_dir)
    : shader_dir(shader_dir)
    , shaders_up_to_date(ATOMIC_FLAG_INIT)
{
    watcher_stopped = std::async(std::launch::async, [&](){watch_shader_dir();});
}

#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUF_LEN (1024 * (EVENT_SIZE + 16))

void shader_loader::watch_shader_dir()
{
    int fd = inotify_init();
    if (fd < 0) {
        throw std::runtime_error((std::string("error initializing inotify: ") + std::to_string(errno)).c_str());
    }
    int wd = inotify_add_watch(fd, shader_dir.c_str(), IN_MODIFY | IN_CREATE | IN_DELETE);
    if (wd < 0) {
        throw std::runtime_error((std::string("error setting watch for dir ")
                                     + shader_dir + std::string(" ") + std::to_string(errno)).c_str());
    }

    while (watcher_should_run) {
        char buf[BUF_LEN];
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

void shader_loader::update_shaders()
{
    if (shaders_up_to_date.test_and_set()) {
        return;
    }
}

void shader_loader::stop()
{
}

void shader_loader::start()
{
}

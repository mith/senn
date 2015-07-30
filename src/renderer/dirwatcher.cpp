#include "dirwatcher.hpp"

#include <sys/inotify.h>
#include <stdexcept>
#include <unistd.h>
#include <poll.h>
#include <string.h>
#include <iostream>
#include <mutex>

DirWatcher::DirWatcher(const std::string& dir)
    : dir(dir)
    , dir_not_changed(ATOMIC_FLAG_INIT)
{
    dir_not_changed.test_and_set();
}

void DirWatcher::watch_dir()
{
    std::cout << "start watching dir: " << dir << std::endl;
    int fd = inotify_init();
    if (fd < 0) {
        throw std::runtime_error(std::string("error initializing inotify: ") + strerror(errno));
    }
    int wd = inotify_add_watch(fd, dir.c_str(), IN_MODIFY | IN_CREATE);
    if (wd < 0) {
        throw std::runtime_error(std::string("error setting watch for dir ")
                                     + dir + " " + strerror(errno));
    }

    const size_t event_size = sizeof(inotify_event);
    const size_t buf_len = 1024 * (event_size + 16);

    pollfd pfd = { fd, POLLIN, 0 };
    char buf[buf_len];

    while (watcher_should_run) {
        int ret = poll(&pfd, 1, 500);
        if (ret < 0) {
            std::cerr << "dir poll failed: " << strerror(errno) << std::endl;
        } else if (ret > 0) {
          int len = read(fd, buf, buf_len);
          int i = 0;
          while(i < len) {
              inotify_event* event;
              event = (inotify_event*)&buf[i];
              dir_not_changed.clear();
              i += event_size + event->len;
          }
        }
    }

    std::cout << "stop watching dir: " << dir << std::endl;

    inotify_rm_watch(fd, wd);
    close(fd);
    watcher_stopped.notify_one();
}

void DirWatcher::start()
{
    watcher_should_run = true;
    std::thread([&]() {watch_dir();}).detach();
}

void DirWatcher::stop() 
{
    watcher_should_run = false;
    std::unique_lock<std::mutex> lk(m);
    watcher_stopped.wait(lk);
}

bool DirWatcher::dir_changed()
{
    return !dir_not_changed.test_and_set();
}

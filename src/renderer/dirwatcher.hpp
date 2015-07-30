#pragma once

#include <thread>
#include <string>
#include <atomic>
#include <condition_variable>
#include <mutex>

class DirWatcher {
    const std::string dir;

    std::atomic_flag dir_not_changed;
    std::atomic<bool> watcher_should_run;
    std::mutex m;
    std::condition_variable watcher_stopped;

    void watch_dir();

public:
    DirWatcher(const std::string& dir);

    void start();
    void stop();
    bool dir_changed();
};

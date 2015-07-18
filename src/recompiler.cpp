#include <sys/types.h>
#include <sys/inotify.h>
#include <cerrno>
#include <unistd.h>
#include <chrono>
#include <iostream>
#include <dlfcn.h>
#include <exception>
#include <system_error>
#include <uuid/uuid.h>

#include "recompiler.hpp"

recompiler::recompiler(const std::string & lib_name)
    : source_dir("../src")
    , lib_name(lib_name)
    , lib_filename ("lib" + lib_name + ".so")
{
    fd = inotify_init();
    if (fd < 0) {
        std::cerr << "error initializing inotify: " << errno << std::endl;
    }
    std::string watch_dir = source_dir + "/" + lib_name;
    wd = inotify_add_watch(fd, 
                           watch_dir.c_str(), 
                           IN_MODIFY | IN_CREATE | IN_DELETE);
    if (wd < 0) {
        std::cerr << "error setting watch: " << errno << std::endl;
        std::cerr << "attempted watch dir: " << watch_dir << std::endl;
    }
    watcher = std::thread([&](void){
        bool recompile_needed = true;
        while(true) {
            char buf[BUF_LEN];
            int len = read(fd, buf, BUF_LEN);
            if (len < 0) {
                if (errno == EINTR) {
                } else {
                    std::cerr << "error reading inotify events: " << errno << std::endl;
                }
            }
            int i = 0;
            while (i < len) {
                struct inotify_event * event;
                event = (struct inotify_event*) &buf[i];
                recompile_needed = true;
                i += EVENT_SIZE + event->len;
            }

            if (recompile_needed && !new_lib.valid()) {
                new_lib = std::async(std::launch::async, [&](){
                    if (unlink(lib_filename.c_str()) < 0) {
                        std::cerr << "error removing old lib: " << errno << std::endl;
                    }
                    if (current_lib.handle != nullptr) {
                        unload(current_lib.handle);
                    }
                    compile();
                    return link();
                });
                recompile_needed = false;
            }
        }
    });
    current_lib = link();
}

lib_functions recompiler::refresh_lib()
{
    if (new_lib.valid()) {
        auto status = new_lib.wait_for(std::chrono::seconds(0));
        if (status == std::future_status::ready) {
            current_lib = new_lib.get();
        }
    }
    return current_lib.functions;
}

void recompiler::unload(void * lib_handle)
{
    int err = dlclose(lib_handle);
    if (err != 0) {
        std::cerr << "error unloading lib: " << dlerror() << std::endl;
    }
}

void recompiler::compile()
{
    int err = std::system("ninja");
    if (err != 0) {
        throw std::runtime_error("failure compiling");
    }
}

linked_lib recompiler::link()
{
    linked_lib new_lib;
    new_lib.handle = dlopen(("./" + lib_filename).c_str(), RTLD_LAZY);
    if (new_lib.handle == nullptr) {
        throw std::runtime_error(std::string(dlerror()));
    }
    *(void**)(&new_lib.functions.init) = dlsym(new_lib.handle, "init");
    *(void**)(&new_lib.functions.update) = dlsym(new_lib.handle, "update");
    *(void**)(&new_lib.functions.tick) = dlsym(new_lib.handle, "tick");
    return new_lib;
}

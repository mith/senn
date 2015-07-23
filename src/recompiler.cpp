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
#include <stdlib.h>
#include <cstring>

#include "recompiler.hpp"

recompiler::recompiler(const std::string& lib_name)
    : source_dir("../src")
    , lib_name(lib_name)
    , lib_filename("./" + lib_name + "/lib" + lib_name + ".so")
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
    watcher = std::thread([&](void) {
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
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                    recompile_needed = false;
                    std::cout << "recompiling" << std::endl;
                    try {
                        compile();
                        return std::experimental::make_optional(link());
                    } catch (std::exception & e) {
                        return std::experimental::optional<linked_lib>();
                    }
                });
            }
        }
    });
    current_lib = link();
}

recompiler::~recompiler()
{
    remove(current_lib.filename);
}

bool recompiler::refresh_lib(lib_functions& fns)
{
    if (fns.init == nullptr) {
        fns = current_lib.functions;
    }
    if (new_lib.valid()) {
        auto status = new_lib.wait_for(std::chrono::seconds(0));
        if (status == std::future_status::ready) {
            auto m_lib = new_lib.get();
            if (m_lib) {
                unload(current_lib);
                current_lib = m_lib.value();
                fns = current_lib.functions;
                return true;
            }
        }
    }
    return false;
}

void recompiler::unload(linked_lib& lib)
{
    int err = dlclose(lib.handle);
    if (err != 0) {
        std::cerr << "error unloading lib: " << dlerror() << std::endl;
    }
    std::cout << "removing " << lib.filename << std::endl;
    remove(lib.filename);
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
    auto tmpfilename = lib_filename + "XXXXXX";
    new_lib.filename = new char[tmpfilename.length() + 1];
    strcpy(new_lib.filename, tmpfilename.c_str());
    int tmpfile = mkstemp(new_lib.filename);
    close(tmpfile);
    std::system(("cp " + lib_filename + " " + new_lib.filename).c_str());
    std::cout << new_lib.filename << std::endl;
    new_lib.handle = dlopen(new_lib.filename, RTLD_LAZY);
    std::cout << "new handle: " << new_lib.handle << std::endl;
    if (new_lib.handle == nullptr) {
        throw std::runtime_error(std::string(dlerror()));
    }
    *(void**)(&new_lib.functions.init) = dlsym(new_lib.handle, "init");
    *(void**)(&new_lib.functions.update) = dlsym(new_lib.handle, "update");
    *(void**)(&new_lib.functions.tick) = dlsym(new_lib.handle, "tick");
    return new_lib;
}

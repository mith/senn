#include "recompiler.hpp"

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

Recompiler::Recompiler(const std::string& lib_name)
    : source_dir("../src")
    , lib_name(lib_name)
    , lib_filename("./" + lib_name + "/lib" + lib_name + ".so")
{
    fd = inotify_init();
    if (fd < 0) {
        throw std::runtime_error(strerror(errno));
    }
    std::string watch_dir = source_dir + "/" + lib_name;
    wd = inotify_add_watch(fd,
        watch_dir.c_str(),
        IN_MODIFY | IN_CREATE | IN_DELETE);
    if (wd < 0) {
        throw std::runtime_error(strerror(errno));
    }
    watcher = std::thread([&](void) {
        bool recompile_needed = true;
        while(true) {
            const size_t event_size = sizeof(inotify_event);
            const size_t buf_len = 1024 * (event_size + 16);

            char buf[buf_len];
            int len = read(fd, buf, buf_len);
            if (len < 0) {
                if (errno == EINTR) {
                } else {
                    std::cerr << "error reading inotify events: " 
                              << strerror(errno) 
                              << std::endl;
                }
            }
            int i = 0;
            while (i < len) {
                struct inotify_event * event;
                event = (struct inotify_event*) &buf[i];
                recompile_needed = true;
                i += event_size + event->len;
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
                        return std::experimental::optional<LinkedLib>();
                    }
                });
            }
        }
    });
    current_lib = link();
}

Recompiler::~Recompiler()
{
    remove(current_lib.filename);
}

bool Recompiler::refresh_lib(LibFunctions& fns)
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

void Recompiler::unload(LinkedLib& lib)
{
    int err = dlclose(lib.handle);
    if (err != 0) {
        std::cerr << "error unloading lib: " << dlerror() << std::endl;
    }
    std::cout << "removing " << lib.filename << std::endl;
    remove(lib.filename);
}

void Recompiler::compile()
{
    int err = std::system("ninja");
    if (err != 0) {
        throw std::runtime_error("failure compiling");
    }
}

LinkedLib Recompiler::link()
{
    LinkedLib new_lib;
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

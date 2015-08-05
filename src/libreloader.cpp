#include "libreloader.hpp"

#include <sys/types.h>
#include <sys/inotify.h>
#include <cerrno>
#include <unistd.h>
#include <chrono>
#include <iostream>
#include <dlfcn.h>
#include <system_error>
#include <stdlib.h>
#include <cstring>

LibReloader::LibReloader(const std::string& source_dir,
                         const std::string& lib_name,
                         const std::string& output_dir,
                         const std::string& build_command)
    : source_dir(source_dir)
    , lib_name(lib_name)
    , output_dir(output_dir)
    , build_command(build_command)
{
    fd = inotify_init();
    if (fd < 0) {
        throw std::runtime_error(strerror(errno));
    }
    std::string watch_dir = source_dir;
    std::cout << "watching dir: " << watch_dir << std::endl;
    wd = inotify_add_watch(fd,
        watch_dir.c_str(),
        IN_MODIFY | IN_CREATE);
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
                    return compile();
                });
            }
        }
    });
}

LibReloader::~LibReloader()
{
    unload(current_lib);
}

std::experimental::optional<std::string> LibReloader::compile()
{
    std::cout << "recompiling" << std::endl;
    try {
        int err = std::system(build_command.c_str());
        if (err != 0) {
            throw std::runtime_error("failure compiling");
        }
        auto tmpfilename = lib_name + "XXXXXX";
        auto tmplib = new char[tmpfilename.length() + 1];
        strcpy(tmplib, tmpfilename.c_str());
        int tmpfile = mkstemp(tmplib);
        close(tmpfile);
        std::system(("cp " + output_dir + "/lib" + lib_name + ".so " + tmplib).c_str());
        std::cout << tmplib << std::endl;

        return std::experimental::make_optional(std::string(tmplib));
    } catch (std::exception & e) {
        return std::experimental::optional<std::string>();
    }

}

bool LibReloader::refresh_lib(LibFunctions& fns, LibState* state)
{
    if (fns.init == nullptr) {
        current_lib = link(compile().value());
        fns = current_lib.functions;
        return true;
    }
    if (new_lib.valid()) {
        auto status = new_lib.wait_for(std::chrono::seconds(0));
        if (status == std::future_status::ready) {
            auto m_lib = new_lib.get();
            if (m_lib) {
                auto old_lib = current_lib;
                current_lib.functions.suspend(state);
                current_lib = link(m_lib.value());
                unload(old_lib);
                fns = current_lib.functions;
                return true;
            }
        }
    }
    return false;
}

void LibReloader::unload(LinkedLib& lib)
{
    int err = dlclose(lib.handle);
    if (err != 0) {
        std::cerr << "error unloading lib: " << dlerror() << std::endl;
    }
    std::cout << "removing " << lib.filename << std::endl;
    remove(lib.filename.c_str());
}

LinkedLib LibReloader::link(const std::string& lib_filename)
{
    LinkedLib new_lib;
    new_lib.filename = lib_filename;
    new_lib.handle = dlopen(("./" + new_lib.filename).c_str(), RTLD_LAZY);
    std::cout << "new handle: " << new_lib.handle << std::endl;
    if (new_lib.handle == nullptr) {
        throw std::runtime_error(std::string(dlerror()));
    }
    *(void**)(&new_lib.functions.init) = dlsym(new_lib.handle, "init");
    if (new_lib.functions.init == nullptr) {
        throw std::runtime_error(std::string(dlerror()));
    }
    *(void**)(&new_lib.functions.suspend) = dlsym(new_lib.handle, "suspend");
    if (new_lib.functions.suspend == nullptr) {
        throw std::runtime_error(std::string(dlerror()));
    }
    *(void**)(&new_lib.functions.resume) = dlsym(new_lib.handle, "resume");
    if (new_lib.functions.resume == nullptr) {
        throw std::runtime_error(std::string(dlerror()));
    }
    *(void**)(&new_lib.functions.tick) = dlsym(new_lib.handle, "tick");
    if (new_lib.functions.tick == nullptr) {
        throw std::runtime_error(std::string(dlerror()));
    }
    *(void**)(&new_lib.functions.finish) = dlsym(new_lib.handle, "finish");
    if (new_lib.functions.finish == nullptr) {
        throw std::runtime_error(std::string(dlerror()));
    }
    return new_lib;
}

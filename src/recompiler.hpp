#include <sys/inotify.h>
#include <thread>
#include <string>
#include <future>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <experimental/optional>

#include "renderer/renderer.hpp"

#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUF_LEN (1024 * (EVENT_SIZE + 16))

typedef void* (*arbitrary)();

struct lib_functions {
    renderer_state* (*init)(GLFWwindow*);
    void (*update)(renderer_state*);
    void (*tick)(renderer_state*);
    lib_functions()
        : init(nullptr)
        , update(nullptr)
        , tick(nullptr)
    {
    }
};

struct linked_lib {
    char* filename;
    void* handle;
    lib_functions functions;
    linked_lib()
        : handle(nullptr)
    {
    }
};

class recompiler {
    int fd;
    int wd;
    std::thread watcher;
    std::future<std::experimental::optional<linked_lib> > new_lib;

    linked_lib current_lib;

    const std::string source_dir;
    const std::string lib_name;
    const std::string lib_filename;

    void compile();
    linked_lib link();
    void unload(linked_lib&);

public:
    recompiler(const std::string& lib_name);
    ~recompiler();
    bool refresh_lib(lib_functions&);
};

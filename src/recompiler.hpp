#include <sys/inotify.h>
#include <thread>
#include <string>
#include <future>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <experimental/optional>

#include "renderer/renderer.hpp"

typedef void* (*arbitrary)();

struct LibFunctions {
    RendererState* (*init)(GLFWwindow*);
    void (*suspend)(RendererState*);
    void (*resume)(RendererState*);
    void (*tick)(RendererState*);
    LibFunctions()
        : init(nullptr)
        , suspend(nullptr)
        , resume(nullptr)
        , tick(nullptr)
    {
    }
};

struct LinkedLib {
    char* filename;
    void* handle;
    LibFunctions functions;
    LinkedLib()
        : handle(nullptr)
    {
    }
};

class Recompiler {
    int fd;
    int wd;
    std::thread watcher;
    std::future<std::experimental::optional<LinkedLib> > new_lib;

    LinkedLib current_lib;

    const std::string source_dir;
    const std::string lib_name;
    const std::string lib_filename;

    void compile();
    LinkedLib link();
    void unload(LinkedLib&);

public:
    Recompiler(const std::string& lib_name);
    ~Recompiler();
    bool refresh_lib(LibFunctions&, RendererState*);
};

#include <sys/inotify.h>
#include <thread>
#include <string>
#include <future>

#include <experimental/optional>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

typedef void* (*arbitrary)();

typedef void LibState;

struct LibFunctions {
    LibState* (*init)(GLFWwindow*);
    void (*suspend)(LibState*);
    void (*resume)(LibState*);
    void (*tick)(LibState*);
    LibFunctions()
        : init(nullptr)
        , suspend(nullptr)
        , resume(nullptr)
        , tick(nullptr)
    {
    }
};

struct LinkedLib {
    std::string filename;
    void* handle;
    LibFunctions functions;
    LinkedLib()
        : handle(nullptr)
    {
    }
};

class LibReloader {
    int fd;
    int wd;
    std::thread watcher;
    std::future<std::experimental::optional<std::string> > new_lib;

    LinkedLib current_lib;

    const std::string source_dir;
    const std::string lib_name;
    const std::string output_dir;
    const std::string build_command;

    std::experimental::optional<std::string> compile();
    LinkedLib link(const std::string&);
    void unload(LinkedLib&);

public:
    LibReloader(const std::string& source_dir, 
                const std::string& output_dir, 
                const std::string& lib_name,
                const std::string& build_command);
    ~LibReloader();
    bool refresh_lib(LibFunctions&, LibState*);
};

#include <sys/inotify.h>
#include <thread>
#include <string>
#include <future>

#define EVENT_SIZE (sizeof (struct inotify_event))
#define BUF_LEN (1024 * (EVENT_SIZE + 16))

typedef void* (*arbitrary)();

struct lib_functions {
    arbitrary init;
    arbitrary update;
    arbitrary tick;
};

struct linked_lib {
    void * handle;
    lib_functions functions;
    linked_lib() : handle(nullptr) {}
};

class recompiler
{
    int fd;
    int wd;
    std::thread watcher;
    std::future<linked_lib> new_lib;

    linked_lib current_lib;

    const std::string source_dir;
    const std::string lib_name;
    const std::string lib_filename;

    void compile();
    linked_lib link();
    void unload(void * lib_handle);

public:
    recompiler(const std::string& lib_name);
    lib_functions refresh_lib();
};

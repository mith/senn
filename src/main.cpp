#include <iostream>
#include <chrono>
#include <thread>

#include "recompiler.hpp"
#include "renderer/renderer.hpp"

typedef void* (*arbitrary)();

int main()
{
    recompiler rc("renderer");
    lib_functions lib_fun = rc.refresh_lib();
    while (true) {
        lib_fun.tick();
        std::this_thread::sleep_for(std::chrono::seconds(2));
        lib_fun = rc.refresh_lib();
    }
    std::cout << "fin" << std::endl;
}

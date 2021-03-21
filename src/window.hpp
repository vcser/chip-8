#ifndef WINDOW_H
#define WINDOW_H

#include <GLFW/glfw3.h>
#include "computer.hpp"

class Renderer;

class Window {
    public:
        Window(const int width, const int height, const char *title);
        void destroy();
        void print(Computer &c8);
        void process_input(Computer &computer);

        GLFWwindow *handle;
    private:
        Renderer *render;
};

namespace Chip_ocho {
    void init();
    void exit();
}

#endif

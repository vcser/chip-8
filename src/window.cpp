#include <glad/glad.h>
#include <iostream>
#include <cstdlib>
#include <string>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "window.hpp"
#include "gfx/renderer.hpp"

extern double delta_time;
static int keymaps[16] = {
    GLFW_KEY_X, GLFW_KEY_1, GLFW_KEY_2, GLFW_KEY_3,
    GLFW_KEY_Q, GLFW_KEY_W, GLFW_KEY_E, GLFW_KEY_A,
    GLFW_KEY_S, GLFW_KEY_D, GLFW_KEY_Z, GLFW_KEY_C,
    GLFW_KEY_4, GLFW_KEY_R, GLFW_KEY_F, GLFW_KEY_V,
};

static void error_callback(int error_code, const char* description) {
    std::cerr << "Error " << error_code << ": " << description << "\n";
}

void Window::process_input(Computer &computer) {
    for (int i = 0; i < 16; i++) {
        if (glfwGetKey(handle, keymaps[i]) == GLFW_PRESS)
            computer.keyboard[i] = 1;
        else
            computer.keyboard[i] = 0;
    }
}

Window::Window(const int width, const int height, const char *title) {
    handle = NULL;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_DOUBLEBUFFER, GL_FALSE);
    handle = glfwCreateWindow(width, height, title, NULL, NULL);
    if (handle == NULL) {
        std::cerr << "Error creating the window\n";
        glfwTerminate();
        std::exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(handle);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);
    render = new Renderer();
}

void Window::destroy() {
    delete render;
    glfwDestroyWindow(handle);
}

void Window::print(Computer &c8) {
    render->draw(c8.display);
    render->gui(c8);
}

void Chip_ocho::init() {
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
        std::cerr << "Error initializing GLFW\n";
        std::exit(EXIT_FAILURE);
    }
}

void Chip_ocho::exit() {
    glfwTerminate();
}

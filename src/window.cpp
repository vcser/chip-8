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

static void error_callback(int error_code, const char* description) {
    std::cerr << "Error " << error_code << ": " << description << "\n";
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
    glfwSwapInterval(0);
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

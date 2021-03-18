#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <iostream>

#if __WIN32
#include <mingw.thread.h> // header to use threads in mingw64
#else
#include <thread>
#endif

#include <chrono>
#include "computer.hpp"
#include "window.hpp"

double delta_time;
double last_time;

int main(int argc, char *argv[]) {
    if (argc > 2)
        std::cerr << "usage: chip-ocho <path-to-rom>\n";

    Computer computer;

    if (argc >= 2)
        computer.load(argv[1]);

    Chip_ocho::init();
    Window window(1280, 660, "CHIP-8");

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window.handle, true);
    ImGui_ImplOpenGL3_Init();

    glViewport(0, 0, 1280, 640);
    glClearColor(0, 0, 0, 0);
    while(!glfwWindowShouldClose(window.handle)) {
        double now = glfwGetTime();
        delta_time = now - last_time;
        last_time = now;

        glClear(GL_COLOR_BUFFER_BIT);

        computer.loop();
        window.print(computer);

        glfwSwapBuffers(window.handle);
        //glFlush();
        glfwPollEvents();
    }

    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();

    window.destroy();
    Chip_ocho::exit();
    return EXIT_SUCCESS;
}

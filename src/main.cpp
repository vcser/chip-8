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
#include <mutex>
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

    std::mutex m;

    std::thread t1(Chip_ocho::check_timers, std::ref(computer));
    std::thread t2(Chip_ocho::loop, std::ref(computer));

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

        window.process_input(computer);
        //computer.loop();
        m.lock();
        window.print(computer);
        m.unlock();

        glfwSwapBuffers(window.handle);
        //glFlush();
        glfwPollEvents();
    }

    computer.state = 3;

    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();

    window.destroy();
    Chip_ocho::exit();
    t1.join();
    t2.join();
    return EXIT_SUCCESS;
}

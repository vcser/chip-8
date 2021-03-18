#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <string>

#include "renderer.hpp"
//#include "shader.hpp"
#include "../computer.hpp"
#include "../util.hpp"

static void cpu_state(Computer &computer);
static void virtual_keyboard(Computer &computer);

static bool show_cpu_state = false;
static bool show_keyboard = false;

extern const double delta_time;

Camera::Camera() {
    projection = glm::ortho(0.0f, 1280.0f, 640.0f, 0.0f, -1.0f, 1.0f);
    position = glm::vec3(0,0,0);
    update_vectors();
}

void Camera::update_vectors() {
    glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * 
        glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0.0f,0.0f,1.0f));
    view = glm::inverse(transform);
}

Renderer::Renderer() {
    shader = Shader("res/shaders/vertex.vs", "res/shaders/fragment.fs");
    float square[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,

        0.0f, 1.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
    };

    white = glm::vec3(1,1,1);
    black = glm::vec3(0,0,0);

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(square), square, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, false, 2*sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
}

Renderer::~Renderer() {

}

void Renderer::draw(Display *display) {
    glClearColor(black.x, black.y, black.z, 1);
    shader.use();

    glm::mat4 model(1.0f);
    shader.set_uniform("view", camera.get_view());
    shader.set_uniform("projection", camera.get_projection());

    glBindVertexArray(vao);
    for (int k = 0; k < 32; k++) {
        for (int j = 0; j < 64; j++) {
            bool pixel = display->pixels[k*64+j];
            if (pixel) {
                model = glm::translate(glm::mat4(1), glm::vec3(j*20, k*20, 0)) *
                    glm::scale(glm::mat4(1.0f), glm::vec3(20.0f, 20.0f, 1.0f));
                shader.set_uniform("model", model);
                shader.set_uniform("color", white);
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
        }
    }
    glBindVertexArray(0);
}

void Renderer::gui(Computer &computer) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (show_cpu_state) cpu_state(computer);
    if (show_keyboard) virtual_keyboard(computer);

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            ImGui::Text("a");
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::BeginMenu("Colors")) {
                ImGui::ColorEdit3("Pixel", &white[0]);
                ImGui::ColorEdit3("Background", &black[0]);
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View")) {
            ImGui::Checkbox("CPU state", &show_cpu_state);
            ImGui::Checkbox("Keyboard", &show_keyboard);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Emulation")) {
            if (computer.state == 0) {
                if (ImGui::Button("Pause"))
                    computer.state = 2;
            } else {
                if (ImGui::Button("Run"))
                    computer.state = 0;
            }
            if (ImGui::Button("Step"))
                computer.state = 1;
            if (ImGui::Button("Reset"))
                computer.reset();

            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

static void cpu_state(Computer &computer) {
    if (ImGui::Begin("CPU-State", &show_cpu_state)) {
        if (ImGui::BeginTable("table", 2, ImGuiTableFlags_Borders)) {
            ImGui::TableNextColumn();
            ImGui::Text("step count:");
            ImGui::TableNextColumn();
            ImGui::Text("%lld", computer.step_count);
            ImGui::EndTable();
        }

        if (ImGui::BeginTable("table2", 4, ImGuiTableFlags_Borders)) {
            ImGui::TableNextColumn();
            ImGui::Text("PC");
            ImGui::Text("%d", computer.pc);
            ImGui::TableNextColumn();
            ImGui::Text("I");
            ImGui::Text("%d", computer.i);
            ImGui::TableNextColumn();
            ImGui::Text("DT");
            ImGui::Text("%d", computer.dt);
            ImGui::TableNextColumn();
            ImGui::Text("ST");
            ImGui::Text("%d", computer.st);
            ImGui::EndTable();
        }

        if (ImGui::BeginTable("table3", 8, ImGuiTableFlags_Borders)) {
            char text[3];
            ImGui::TableNextColumn();
            for (int i = 0; i < 16; i++) {
                sprintf(text, "V%X", i);
                ImGui::Text("%s", text);
                ImGui::Text("%d", computer.v[i]);
                if (i != 15) ImGui::TableNextColumn();
            }
            ImGui::EndTable();
        }

        char key_pressed[2] = " ";
        for (int i = 0; i < 16; i++)
            if (computer.keyboard[i])
                std::sprintf(key_pressed, "%X", i);
        
        ImGui::Text("Key: %s", key_pressed);

        ImGui::Text("SP: %X", computer.sp);
        if (ImGui::CollapsingHeader("Stack", true)) {
            if (ImGui::BeginTable("stack_table", 2, ImGuiTableFlags_Borders)) {
                char text[4];
                ImGui::TableNextColumn();
                for (int i = 0; i < 16; i++) {
                    sprintf(text, "%X", computer.stack[i]);
                    ImGui::Text("Depth%d", i);
                    ImGui::TableNextColumn();
                    ImGui::Text("0x%s", text);
                    if (i != 15) ImGui::TableNextColumn();
                }
                ImGui::EndTable();
            }
        }
    }
    ImGui::End();
}

static void virtual_keyboard(Computer &computer) {
    if (ImGui::Begin("Keyboard", &show_keyboard)) {
        char text[2];
        for (int i = 1; i <= 16; i++) {
            std::sprintf(text, "%X", i-1);
            if (ImGui::Button(text, ImVec2(50,50)) || ImGui::IsItemActive())
                computer.keyboard[i-1] = 1;
            else
                computer.keyboard[i-1] = 0;
            if (i % 4)
                ImGui::SameLine();
        }
    }
    ImGui::End();
}

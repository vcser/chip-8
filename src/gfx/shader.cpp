#include <cstdio>
#include <cstring>
#include <iostream>
#include <glad/glad.h>
#include "shader.hpp"

enum ShaderType {
    VERTEX,
    FRAGMENT,
    PROGRAM,
};

static void file_to_str(const char *path, char *&str) {
    FILE *file = std::fopen(path, "r");
    if (!file) {
        std::cerr << "Error al abrir el archivo: " << path << "\n";
        return;
    }
    std::fseek(file, 0, SEEK_END);
    unsigned int size = std::ftell(file);
    str = new char[size + 1];
    std::memset(str, '\0', size + 1);
    std::rewind(file);
    std::fread((void *) str, sizeof(char), size, file);
    std::fclose(file);
    str[size] = '\0';
    //std::cout << str << "\n";
}

Shader::Shader(const char *vertex_path, const char *fragment_path) {
    char *vertex_src = NULL;
    char *fragment_src = NULL;
    file_to_str(vertex_path, vertex_src);
    file_to_str(fragment_path, fragment_src);

    unsigned int vertex, fragment;
    
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertex_src, NULL);
    glCompileShader(vertex);
    check_errors(vertex, VERTEX);

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragment_src, NULL);
    glCompileShader(fragment);
    check_errors(fragment, FRAGMENT);
    
    id = glCreateProgram();
    glAttachShader(id, vertex);
    glAttachShader(id, fragment);
    glLinkProgram(id);
    check_errors(id, PROGRAM);

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    delete [] vertex_src;
    delete [] fragment_src;
}

Shader::Shader() {
    id = 0;
}

void Shader::set_uniform(const char *name, glm::mat4 &mat4) {
    glUniformMatrix4fv(glGetUniformLocation(id, name), 1, false, &mat4[0][0]);
}

void Shader::set_uniform(const char *name, glm::vec3 vec3) {
    glUniform3f(glGetUniformLocation(id, name), vec3.x, vec3.y, vec3.z);
}

void Shader::use() {
    glUseProgram(id);
}

void Shader::check_errors(unsigned int shader, unsigned int type) {
    int status;
    if (type != PROGRAM) {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        if (!status) {
            char message[1024];
            char source[1024];
            glGetShaderInfoLog(shader, 1024, NULL, message);
            std::cerr << "ERROR " << type << " " << message << "\n";
            glGetShaderSource(shader, 1024, NULL, source);
            std::cerr << "SOURCE " << type << "\n" << source << "\n";
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &status);
        if (!status) {
            char message[1024];
            glGetProgramInfoLog(shader, 1024, NULL, message);
            std::cerr << "ERROR " << type << ": " << message << "\n"; 
        }
    }
}

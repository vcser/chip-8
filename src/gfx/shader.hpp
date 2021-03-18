#ifndef SHADER_H
#define SHADER_H

#include <glm/glm.hpp>

class Shader {
    public:
        Shader(const char *vertex_path, const char *fragment_path);
        Shader();
        void use();
        void set_uniform(const char *name, glm::mat4 &mat4);
        void set_uniform(const char *name, glm::vec3 vec3);
    private:
        unsigned int id;
        void check_errors(unsigned int shader, unsigned int type);
};

#endif

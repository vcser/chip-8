#ifndef RENDERER_H
#define RENDERER_H

#include "shader.hpp"

struct Display;
//class Shader;
class Computer;

class Camera {
    public:
        Camera();
        void update_vectors();
        glm::mat4 &get_view() {return view;};
        glm::mat4 &get_projection() {return projection;};
        void set_position(glm::vec3 &vec3) {position = vec3; update_vectors();};
    private:
        glm::mat4 projection;
        glm::mat4 view;
        glm::vec3 position;
};

class Renderer {
    public:
        Renderer();
        ~Renderer();
        void draw(Display *display);
        void gui(Computer &computer);

        Camera camera;
    private:
        Shader shader;
        unsigned int vao;
        unsigned int vbo;
        glm::vec3 white;
        glm::vec3 black;
};

#endif

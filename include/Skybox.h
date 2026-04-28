#ifndef SKYBOX_H
#define SKYBOX_H

#include "ShaderProgram.h"

#include <glad/glad.h>

class Skybox {
  public:
    Skybox(ShaderProgram shader);
    void render(glm::mat4 sky_view, float current_frame);

  private:
    GLuint vao_;
    GLuint vbo_;
    ShaderProgram shader_;
};

#endif

#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>

class Shader {

  private:

    unsigned int shader_ref_;

    std::string readCode(const std::string& filepath);

  public:

    Shader(GLenum shader_type, const std::string& filepath);
    
    unsigned int getShaderRef();

    void deleteShader();
};

#endif

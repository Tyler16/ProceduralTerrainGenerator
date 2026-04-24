#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include "Shader.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>

class ShaderProgram {
  public:
    ShaderProgram();
    void addShader(Shader& shader);
    void build();
    void use();
    void setVec3(const std::string& name, const glm::vec3& value);
    void setMat3(const std::string& name, const glm::mat3& mat);
    void setMat4(const std::string& name, const glm::mat4& mat);
    void setFloat(const std::string& name, float value);
    
  private:
    unsigned int program_id_;
    std::vector<Shader> shaders_;
};

#endif

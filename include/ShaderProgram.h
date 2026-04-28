#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include "Shader.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <vector>

class ShaderProgram {
  public:
    ShaderProgram();
    ShaderProgram(const std::string& vertex_path,
                  const std::string& fragment_path);
    void addShader(Shader& shader);
    void build();
    void use();
    void setVec3(const std::string& name, const glm::vec3& value);
    void setMat3(const std::string& name, const glm::mat3& mat);
    void setMat4(const std::string& name, const glm::mat4& mat);
    void setFloat(const std::string& name, float value);
    unsigned int getID();
    
  private:
    unsigned int program_id_;
    std::vector<Shader> shaders_;
};

#endif

#include "ShaderProgram.h"

#include "Shader.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>

ShaderProgram::ShaderProgram() {
   program_id_ = glCreateProgram(); 
}

void ShaderProgram::addShader(Shader& shader) {
    shaders_.push_back(shader);
}

void ShaderProgram::build() {
    for (Shader& shader : shaders_) {
       glAttachShader(program_id_, shader.getShaderRef());
    }

    glLinkProgram(program_id_);

    for (Shader& shader : shaders_) {
        shader.deleteShader();
    }
}

void ShaderProgram::use() {
    glUseProgram(program_id_);
}

void ShaderProgram::setVec3(const std::string& name, const glm::vec3& value) {
    int location = glGetUniformLocation(program_id_, name.c_str());
    if (location != -1) {
        glUniform3fv(location, 1, &value[0]);
    }
}

void ShaderProgram::setMat3(const std::string& name, const glm::mat3& value) {
    int location = glGetUniformLocation(program_id_, name.c_str());
    if (location != -1) {
        glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }
}

void ShaderProgram::setMat4(const std::string& name, const glm::mat4& value) {
    int location = glGetUniformLocation(program_id_, name.c_str());
    if (location != -1) {
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }
}

void ShaderProgram::setFloat(const std::string& name, float value) {
    int location = glGetUniformLocation(program_id_, name.c_str());
    if (location != -1) {
        glUniform1f(location, value);
    }
}

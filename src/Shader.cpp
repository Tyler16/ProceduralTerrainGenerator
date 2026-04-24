#include "Shader.h"

#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>


Shader::Shader(GLenum shader_type, const std::string& filepath) {
    std::string code = readCode(filepath);
    const char* c_str_code = code.c_str();
    shader_ref_ = glCreateShader(shader_type);
    glShaderSource(shader_ref_, 1, &c_str_code, NULL);
    glCompileShader(shader_ref_);
}

std::string Shader::readCode(const std::string& filepath) {
    if (!std::filesystem::exists(filepath)) {
        std::cerr << "ERROR: Shader files not found\nFilepath: " << filepath << std::endl;
        return "";
    }

    std::ifstream file;
    file.open(filepath);
    std::stringstream stream;
    stream << file.rdbuf();
    file.close();
    return stream.str();
}

unsigned int Shader::getShaderRef() {
    return shader_ref_;
}

void Shader::deleteShader() {
    glDeleteShader(shader_ref_);
    shader_ref_ = 0;
}


//
// Created by Artyom Dangizyan on 11/13/18.
//
#pragma once

#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <unordered_map>

class Shader {

private:


    std::string m_vertexShaderPath;
    std::string m_fragmentShaderPath;

    std::unordered_map<std::string, int> m_locationUniformCache;

    GLint getUniformLocation(const std::string &name);

    GLuint compileShader(GLuint type, std::string &source);

    std::string parseFile(const std::string &path);

    GLuint createShader(std::string &vertexShader, std::string &fragmentShader);

public:
    unsigned int m_RendererId;

    Shader(const std::string &vertexShaderPath, const std::string &fragmentShaderPath);

    ~Shader();

    void bind() const;

    void unbind() const;

    void setUniform4f(const std::string &name, float v0, float v1, float v2, float v3);

    void setUniformMatrix4f(const std::string &name, glm::mat4 &matrix);

    void setUniform1i(const std::string &name, int value);

    void setUniform1f(const std::string &name, float value);

};

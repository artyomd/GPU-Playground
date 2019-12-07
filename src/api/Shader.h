//
// Created by Artyom Dangizyan on 11/13/18.
//
#pragma once

#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <unordered_map>
#include <memory>
#include "ShaderProperty.h"

class Shader {

private:
    std::string m_vertexShaderPath;

    std::string m_fragmentShaderPath;

    std::unordered_map<std::string, std::unique_ptr<ShaderProperty>> uniforms;

    std::unordered_map<std::string, int> m_locationUniformCache;

    GLint getUniformLocation(const std::string &name);

    static GLuint compileShader(GLuint type, const std::string &source);

    static std::string parseFile(const std::string &path);

    static GLuint createShader(const std::string &vertexShader, const std::string &fragmentShader);

public:
    unsigned int m_RendererId;

    Shader(const std::string &vertexShaderPath, const std::string &fragmentShaderPath);

    ~Shader();

    void bind() const;

    void unbind() const;

    void recompile();

    void setUniform(const std::string &name, ShaderProperty *shaderProperty);

};

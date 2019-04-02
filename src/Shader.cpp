//
// Created by Artyom Dangizyan on 11/13/18.
//

#include <fstream>
#include <sstream>
#include <iostream>

#include "Shader.h"
#include "Renderer.h"

Shader::Shader(const std::string &vertexShaderPath, const std::string &fragmentShaderPath)
        : m_vertexShaderPath(vertexShaderPath), m_fragmentShaderPath(fragmentShaderPath), m_RendererId(0) {
    m_RendererId = createShader(parseFile(vertexShaderPath), parseFile(fragmentShaderPath));
}

Shader::~Shader() {
    GLCall(glUseProgram(0));
    GLCall(glDeleteProgram(m_RendererId));
}

void Shader::bind() const {
    GLCall(glUseProgram(m_RendererId));
}

void Shader::unbind() const {
    GLCall(glUseProgram(0));
}

void Shader::setUniform(const std::string &name, ShaderProperty *shaderProperty) {
    shaderProperty->apply(getUniformLocation(name));
    uniforms[name] = shaderProperty->clone();
}

GLint Shader::getUniformLocation(const std::string &name) {
    if (m_locationUniformCache.find(name) != m_locationUniformCache.end()) {
        return m_locationUniformCache[name];
    }
    GLCall(GLint uniformLocation = glGetUniformLocation(m_RendererId, name.c_str()));
    if (uniformLocation == -1) {
        std::cout << "Warning: uniform '" << name << "' doesn't exists!" << std::endl;
    }
    m_locationUniformCache[name] = uniformLocation;
    return uniformLocation;
}

GLuint Shader::compileShader(GLuint type, const std::string &source) {
    GLCall(GLuint id = glCreateShader(type));
    const char *src = source.c_str();
    GLCall(glShaderSource(id, 1, &src, nullptr));
    GLCall(glCompileShader(id));

    int result;
    GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));

    if (result == GL_FALSE) {
        int length;
        GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
        char *message = (char *) alloca(length * sizeof(char));
        GLCall(glGetShaderInfoLog(id, length, &length, message));
        std::cout << "Failed to compile shader of type:" << type << std::endl;
        std::cout << "message:" << message;
        GLCall(glDeleteShader(id));
        return 0;
    }

    return id;
}

std::string Shader::parseFile(const std::string &path) {
    std::fstream stream(path);
    std::string line;

    std::stringstream stringStream;
    while (getline(stream, line)) {
        stringStream << line << "\n";
    }

    return stringStream.str();
}

void Shader::recompile() {
    unbind();
    GLCall(glDeleteProgram(m_RendererId));
    m_RendererId = createShader(parseFile(m_vertexShaderPath), parseFile(m_fragmentShaderPath));
    bind();
    for (auto &uniform : uniforms) {
        uniform.second->apply(getUniformLocation(uniform.first));
    }
    unbind();
}


void getProgramInfoLog(int id) {
    int length;
    GLCall(glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length));
    if (length != 0) {
        char *message = (char *) alloca(length * sizeof(char));
        GLCall(glGetProgramInfoLog(id, length, &length, message));
        std::cout << "Program creation info message:" << message;
    }
}

GLuint Shader::createShader(const std::string &vertexShader, const std::string &fragmentShader) {
    GLCall(GLuint program = glCreateProgram());

    getProgramInfoLog(program);

    GLCall(GLuint vs = compileShader(GL_VERTEX_SHADER, vertexShader));
    GLCall(GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader));

    GLCall(glAttachShader(program, vs));

    getProgramInfoLog(program);

    GLCall(glAttachShader(program, fs));

    getProgramInfoLog(program);
    GLCall(glLinkProgram(program));

    getProgramInfoLog(program);
    GLCall(glValidateProgram(program));

    getProgramInfoLog(program);
    GLCall(glDeleteShader(vs));
    GLCall(glDeleteShader(fs));

    return program;
}


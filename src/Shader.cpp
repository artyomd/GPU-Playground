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
    std::string vertexShader = parseFile(vertexShaderPath);
    std::string fragmentShader = parseFile(fragmentShaderPath);

    m_RendererId = createShader(vertexShader, fragmentShader);
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

void Shader::setUniform1i(const std::string &name, int value) {
    GLCall(glUniform1i(getUniformLocation(name), value));
}

void Shader::setUniform1f(const std::string &name, float value) {
    GLCall(glUniform1f(getUniformLocation(name), value));
}

void Shader::setUniform2f(const std::string &name, float v0, float v1) {
    GLCall(glUniform2f(getUniformLocation(name), v0, v1));
}

void Shader::setUniform2i(const std::string &name, int v0, int v1) {
    GLCall(glUniform2i(getUniformLocation(name), v0, v1));
}

void Shader::setUniform4f(const std::string &name, float v0, float v1, float v2, float v3) {
    GLCall(glUniform4f(getUniformLocation(name), v0, v1, v2, v3));
}

void Shader::setUniformMatrix4f(const std::string &name, glm::mat4 &matrix) {
    GLCall(glUniformMatrix4fv(getUniformLocation(name), 1, false, &matrix[0][0]));
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

GLuint Shader::compileShader(GLuint type, std::string &source) {
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


GLuint Shader::createShader(std::string &vertexShader, std::string &fragmentShader) {
    GLCall(GLuint program = glCreateProgram());

    GLCall(GLuint vs = compileShader(GL_VERTEX_SHADER, vertexShader));
    GLCall(GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader));

    GLCall(glAttachShader(program, vs));
    GLCall(glAttachShader(program, fs));
    GLCall(glLinkProgram(program));
    GLCall(glValidateProgram(program));

    GLCall(glDeleteShader(vs));
    GLCall(glDeleteShader(fs));

    return program;
}

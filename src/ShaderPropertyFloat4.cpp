//
// Created by artyomd on 4/1/19.
//

#include <GL/glew.h>
#include "ShaderPropertyFloat4.h"
#include "Renderer.h"

void ShaderPropertyFloat4::apply(int uniformLocation) {
    GLCall(glUniform4f(uniformLocation, m_value1, m_value2, m_value3, m_value4));
}

ShaderPropertyFloat4::ShaderPropertyFloat4(float value1, float value2, float value3, float value4)
        : m_value1(value1), m_value2(value2), m_value3(value3), m_value4(value4) {

}

float ShaderPropertyFloat4::getValue1() const {
    return m_value1;
}

void ShaderPropertyFloat4::setValue1(float value) {
    m_value1 = value;
}

float ShaderPropertyFloat4::getValue2() const {
    return m_value2;
}

void ShaderPropertyFloat4::setValue2(float value) {
    m_value2 = value;
}

float ShaderPropertyFloat4::getValue3() const {
    return m_value3;
}

void ShaderPropertyFloat4::setValue3(float value) {
    m_value3 = value;
}

float ShaderPropertyFloat4::getValue4() const {
    return m_value4;
}

void ShaderPropertyFloat4::setValue4(float value) {
    m_value4 = value;
}

std::unique_ptr<ShaderProperty> ShaderPropertyFloat4::clone() const {
    return std::make_unique<ShaderPropertyFloat4>(*this);
}

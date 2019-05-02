//
// Created by artyomd on 4/1/19.
//

#include <GL/glew.h>
#include "ShaderPropertyFloat4.h"
#include "Renderer.h"

void ShaderPropertyFloat4::apply(int uniformLocation) {
    GLCall(glUniform4f(uniformLocation, m_values[0], m_values[1], m_values[2], m_values[3]));
}

ShaderPropertyFloat4::ShaderPropertyFloat4(float value0, float value1, float value2, float value3){
    setValue0(value0);
    setValue1(value1);
    setValue2(value2);
    setValue3(value3);
}

float ShaderPropertyFloat4::getValue0() const {
    return m_values[0];
}

void ShaderPropertyFloat4::setValue0(float value) {
    m_values[0] = value;
}

float ShaderPropertyFloat4::getValue1() const {
    return m_values[1];
}

void ShaderPropertyFloat4::setValue1(float value) {
    m_values[1] = value;
}

float ShaderPropertyFloat4::getValue2() const {
    return m_values[2];
}

void ShaderPropertyFloat4::setValue2(float value) {
    m_values[2] = value;
}

float ShaderPropertyFloat4::getValue3() const {
    return m_values[3];
}

void ShaderPropertyFloat4::setValue3(float value) {
    m_values[3] = value;
}

std::unique_ptr<ShaderProperty> ShaderPropertyFloat4::clone() const {
    return std::make_unique<ShaderPropertyFloat4>(*this);
}

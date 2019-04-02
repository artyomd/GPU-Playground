//
// Created by artyomd on 4/1/19.
//

#include <GL/glew.h>
#include "ShaderPropertyFloat2.h"
#include "Renderer.h"

void ShaderPropertyFloat2::apply(int uniformLocation) {
    GLCall(glUniform2f(uniformLocation, m_value1, m_value2));
}

ShaderPropertyFloat2::ShaderPropertyFloat2(float value1, float value2) : m_value1(value1), m_value2(value2) {

}

float ShaderPropertyFloat2::getValue1() const {
    return m_value1;
}

void ShaderPropertyFloat2::setValue1(float value) {
    m_value1 = value;
}

float ShaderPropertyFloat2::getValue2() const {
    return m_value2;
}

void ShaderPropertyFloat2::setValue2(float value) {
    m_value2 = value;
}

std::unique_ptr<ShaderProperty> ShaderPropertyFloat2::clone() const {
    return std::make_unique<ShaderPropertyFloat2>(*this);
}
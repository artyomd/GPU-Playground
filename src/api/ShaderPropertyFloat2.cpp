//
// Created by artyomd on 4/1/19.
//

#include <GL/glew.h>
#include "ShaderPropertyFloat2.h"
#include "Renderer.h"

void ShaderPropertyFloat2::apply(int uniformLocation) {
    GLCall(glUniform2f(uniformLocation, m_values[0], m_values[1]));
}

ShaderPropertyFloat2::ShaderPropertyFloat2(float value0, float value1){
    setValue0(value0);
    setValue1(value1);
}

float ShaderPropertyFloat2::getValue0() const {
    return m_values[0];
}

void ShaderPropertyFloat2::setValue0(float value) {
    m_values[0] = value;
}

float ShaderPropertyFloat2::getValue1() const {
    return m_values[1];
}

void ShaderPropertyFloat2::setValue1(float value) {
    m_values[1] = value;
}

std::unique_ptr<ShaderProperty> ShaderPropertyFloat2::clone() const {
    return std::make_unique<ShaderPropertyFloat2>(*this);
}

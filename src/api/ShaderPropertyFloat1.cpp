//
// Created by artyomd on 4/1/19.
//

#include <GL/glew.h>
#include "ShaderPropertyFloat1.h"
#include "Renderer.h"

void ShaderPropertyFloat1::apply(int uniformLocation) {
    GLCall(glUniform1f(uniformLocation, m_value));
}

ShaderPropertyFloat1::ShaderPropertyFloat1(float value) : m_value(value) {

}

float ShaderPropertyFloat1::getValue() const {
    return m_value;
}

void ShaderPropertyFloat1::setValue(float value) {
    m_value = value;
}

std::unique_ptr<ShaderProperty> ShaderPropertyFloat1::clone() const {
    return std::make_unique<ShaderPropertyFloat1>(*this);
}

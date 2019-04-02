//
// Created by artyomd on 4/1/19.
//

#include <GL/glew.h>
#include "ShaderPropertyInt1.h"
#include "Renderer.h"

void ShaderPropertyInt1::apply(int uniformLocation) {
    GLCall(glUniform1i(uniformLocation, m_value));
}

ShaderPropertyInt1::ShaderPropertyInt1(int value) : m_value(value) {

}

int ShaderPropertyInt1::getValue() const {
    return m_value;
}

void ShaderPropertyInt1::setValue(int value) {
    m_value = value;
}

std::unique_ptr<ShaderProperty> ShaderPropertyInt1::clone() const {
    return std::make_unique<ShaderPropertyInt1>(*this);
}

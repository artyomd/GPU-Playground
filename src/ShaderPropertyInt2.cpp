//
// Created by artyomd on 4/1/19.
//

#include <GL/glew.h>
#include "Renderer.h"
#include "ShaderPropertyInt2.h"

void ShaderPropertyInt2::apply(int uniformLocation) {
    GLCall(glUniform2i(uniformLocation, m_value1, m_value2));
}

ShaderPropertyInt2::ShaderPropertyInt2(int value1, int value2) : m_value1(value1), m_value2(value2) {

}

int ShaderPropertyInt2::getValue1() const {
    return m_value1;
}

void ShaderPropertyInt2::setValue1(int value) {
    m_value1 = value;
}

int ShaderPropertyInt2::getValue2() const {
    return m_value2;
}

void ShaderPropertyInt2::setValue2(int value) {
    m_value2 = value;
}

std::unique_ptr<ShaderProperty> ShaderPropertyInt2::clone() const {
    return std::make_unique<ShaderPropertyInt2>(*this);
}

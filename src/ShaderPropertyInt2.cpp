//
// Created by artyomd on 4/1/19.
//

#include <GL/glew.h>
#include "Renderer.h"
#include "ShaderPropertyInt2.h"

void ShaderPropertyInt2::apply(int uniformLocation) {
    GLCall(glUniform2i(uniformLocation, m_values[0], m_values[1]));
}

ShaderPropertyInt2::ShaderPropertyInt2(int value0, int value1){
    setValue0(value0);
    setValue1(value1);

}

int ShaderPropertyInt2::getValue0() const {
    return m_values[0];
}

void ShaderPropertyInt2::setValue0(int value) {
    m_values[0] = value;
}

int ShaderPropertyInt2::getValue1() const {
    return m_values[1];
}

void ShaderPropertyInt2::setValue1(int value) {
    m_values[1] = value;
}

std::unique_ptr<ShaderProperty> ShaderPropertyInt2::clone() const {
    return std::make_unique<ShaderPropertyInt2>(*this);
}

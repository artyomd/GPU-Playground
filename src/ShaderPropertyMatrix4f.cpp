//
// Created by artyomd on 4/1/19.
//

#include <GL/glew.h>
#include "Renderer.h"
#include "ShaderPropertyMatrix4f.h"

void ShaderPropertyMatrix4f::apply(int uniformLocation) {
    GLCall(glUniformMatrix4fv(uniformLocation, 1, false, &m_value[0][0]));
}

ShaderPropertyMatrix4f::ShaderPropertyMatrix4f(glm::mat4 &value)
        : m_value(value) {

}

const glm::mat4 &ShaderPropertyMatrix4f::getValue() const {
    return m_value;
}

void ShaderPropertyMatrix4f::setValue(const glm::mat4 &value) {
    m_value = value;
}

std::unique_ptr<ShaderProperty> ShaderPropertyMatrix4f::clone() const {
    return std::make_unique<ShaderPropertyMatrix4f>(*this);
}

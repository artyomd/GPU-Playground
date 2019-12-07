//
// Created by artyomd on 4/1/19.
//

#pragma once

#include "ShaderProperty.h"

class ShaderPropertyMatrix4f : public ShaderProperty {
private:

    glm::mat4 m_value;

public:

    explicit ShaderPropertyMatrix4f(glm::mat4 &Value);

    const glm::mat4 &getValue() const;

    void setValue(const glm::mat4 &mValue);

    void apply(int uniformLocation) override;

    std::unique_ptr<ShaderProperty> clone() const override;
};



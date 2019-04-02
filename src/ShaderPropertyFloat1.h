//
// Created by artyomd on 4/1/19.
//

#pragma once

#include <bits/unique_ptr.h>
#include "ShaderProperty.h"

class ShaderPropertyFloat1 : public ShaderProperty {
private:
    float m_value;

public:

    explicit ShaderPropertyFloat1(float value);

    float getValue() const;

    void setValue(float value);

    void apply(int uniformLocation) override;

    std::unique_ptr<ShaderProperty> clone() const override;

};



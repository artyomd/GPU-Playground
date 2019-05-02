//
// Created by artyomd on 4/1/19.
//

#pragma once

#include <bits/unique_ptr.h>
#include "ShaderProperty.h"

class ShaderPropertyFloat2 : public ShaderProperty {
private:
    float m_values[2] = {0.0f, 0.0f};

public:

    explicit ShaderPropertyFloat2(float value1, float value2);

    float getValue0() const;

    void setValue0(float value);

    float getValue1() const;

    void setValue1(float value);

    void apply(int uniformLocation) override;

    std::unique_ptr<ShaderProperty> clone() const override;

};



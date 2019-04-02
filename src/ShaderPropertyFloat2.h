//
// Created by artyomd on 4/1/19.
//

#pragma once

#include <bits/unique_ptr.h>
#include "ShaderProperty.h"

class ShaderPropertyFloat2 : public ShaderProperty {
private:
    float m_value1;
    float m_value2;

public:

    explicit ShaderPropertyFloat2(float value1, float value2);

    float getValue1() const;

    void setValue1(float value);

    float getValue2() const;

    void setValue2(float value);

    void apply(int uniformLocation) override;

    std::unique_ptr<ShaderProperty> clone() const override;

};



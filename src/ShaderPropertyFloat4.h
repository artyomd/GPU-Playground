//
// Created by artyomd on 4/1/19.
//

#pragma once

#include <bits/unique_ptr.h>
#include "ShaderProperty.h"

class ShaderPropertyFloat4 : public ShaderProperty {
private:
    float m_value1;
    float m_value2;
    float m_value3;
    float m_value4;

public:
    explicit ShaderPropertyFloat4(float value1, float value2, float value3, float value4);

    float getValue1() const;

    void setValue1(float value);

    float getValue2() const;

    void setValue2(float value);

    float getValue3() const;

    void setValue3(float value);

    float getValue4() const;

    void setValue4(float value);

    void apply(int uniformLocation) override;

    std::unique_ptr<ShaderProperty> clone() const override;
};



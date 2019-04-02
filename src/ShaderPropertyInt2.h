//
// Created by artyomd on 4/1/19.
//

#pragma once

#include "ShaderProperty.h"

class ShaderPropertyInt2 : public ShaderProperty {
private:

    int m_value1;
    int m_value2;

public:

    explicit ShaderPropertyInt2(int value1, int value2);

    int getValue1() const;

    void setValue1(int value);

    int getValue2() const;

    void setValue2(int value);

    void apply(int uniformLocation) override;

    std::unique_ptr<ShaderProperty> clone() const override;
};



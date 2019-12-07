//
// Created by artyomd on 4/1/19.
//

#pragma once

#include "ShaderProperty.h"

class ShaderPropertyInt2 : public ShaderProperty {
private:

    int m_values[2] = {0, 0};

public:

    explicit ShaderPropertyInt2(int value0, int value1);

    int getValue0() const;

    void setValue0(int value);

    int getValue1() const;

    void setValue1(int value);

    void apply(int uniformLocation) override;

    std::unique_ptr<ShaderProperty> clone() const override;
};



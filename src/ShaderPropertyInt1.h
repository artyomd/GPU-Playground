//
// Created by artyomd on 4/1/19.
//

#pragma once

#include <bits/unique_ptr.h>
#include "ShaderProperty.h"

class ShaderPropertyInt1 : public ShaderProperty {
private:

    int m_value;

public:
    explicit ShaderPropertyInt1(int value);

    int getValue() const;

    void setValue(int value);

    void apply(int uniformLocation) override;

    std::unique_ptr<ShaderProperty> clone() const override;
};



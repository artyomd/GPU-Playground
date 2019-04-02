//
// Created by artyomd on 4/1/19.
//

#pragma once

#include <memory>

class ShaderProperty {
public:
    virtual void apply(int shaderId) = 0;

    virtual std::unique_ptr<ShaderProperty> clone() const = 0;
};



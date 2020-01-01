*//
// Created by artyomd on 12/31/19.
//
#pragma once

#include "VertexAttributeDescription.cpp"

namespace api {
    class VertexBinding {
    public:
        virtual void addBuffer(const VertexBuffer *buffer, const VertexAttributeDescription *description) = 0;
    };
}
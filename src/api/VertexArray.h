//
// Created by Artyom Dangizyan on 10/21/18.
//
#pragma once

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

class VertexArray {
private:
    unsigned int m_RendererId;
public:
    VertexArray();

    ~VertexArray();

    void addBuffer(const VertexBuffer *vb, const VertexBufferLayout* layout);

    void bind() const;

    void unbind() const;
};

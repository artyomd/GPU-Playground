//
// Created by artyomd on 12/30/19.
//

#pragma once

#include <api/VertexBuffer.h>

namespace api {
    class GlVertexBuffer : public VertexBuffer {
    private:
        unsigned int m_RendererId = -1;
    public:
        GlVertexBuffer(const void *data, unsigned int size);

        ~GlVertexBuffer();

        void bind() const;

        void unbind() const;
    };
}
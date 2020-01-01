//
// Created by Artyom Dangizyan on 10/21/18.
//
#pragma once

#include "VertexBuffer.h"
#include "VertexBinding.h"
#include "VertexAttributeDescription.h"

namespace api {
    class GlVertexBinding : public VertexBinding {
    private:
        unsigned int m_RendererId = -1;
    public:
        GlVertexBinding();

        ~GlVertexBinding();

        void addBuffer(const VertexBuffer *vb, const VertexAttributeDescription *layout);
    };
}
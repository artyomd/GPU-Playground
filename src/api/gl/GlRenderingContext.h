//
// Created by artyomd on 12/31/19.
//
#pragma once

#include <api/RenderingContext.h>

namespace api {
    class GlRenderingContext : public RenderingContext {
        VertexBuffer *createVertexBuffer(const void *data, unsigned int size) override;
        VertexBinding *createVertexBinding() override;
    };
}
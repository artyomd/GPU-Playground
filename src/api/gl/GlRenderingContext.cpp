//
// Created by artyomd on 12/31/19.
//


#include "GlRenderingContext.h"
#include "GlVertexBuffer.h"
#include "GlVertexBinding.h"

namespace api {
    VertexBuffer *GlRenderingContext::createVertexBuffer(const void *data, unsigned int size) {
        return new GlVertexBuffer(data, size);
    }

    VertexBinding *GlRenderingContext::createVertexBinding() {
        return new GlVertexBinding();
    }
}
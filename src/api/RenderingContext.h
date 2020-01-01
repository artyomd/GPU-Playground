//
// Created by artyomd on 12/31/19.
//

#include "VertexBuffer.h"
#include "VertexBinding.h"

namespace api {
    class RenderingContext {
        virtual VertexBuffer *createVertexBuffer(const void *data, unsigned int size) = 0;

        virtual VertexBinding *createVertexBinding() = 0;
    };
}
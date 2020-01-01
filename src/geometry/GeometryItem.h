//
// Created by artyomd on 12/31/18.
//

#pragma once

#include <api/Shader.h>
#include <api/IndexBuffer.h>
#include <api/gl/GlVertexBinding.h>

namespace geometry {
    class GeometryItem {
    public:
        virtual void render(Shader &shader) const = 0;

        virtual ~GeometryItem() {
            delete vertexArray;
            delete indexBuffer;
        }

    protected:
        GlVertexBinding *vertexArray = nullptr;
        IndexBuffer *indexBuffer = nullptr;
    };
}
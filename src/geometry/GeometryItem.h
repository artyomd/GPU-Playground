//
// Created by artyomd on 12/31/18.
//

#pragma once

#include <Shader.h>
#include <IndexBuffer.h>
#include <VertexArray.h>

class GeometryItem {
public:
    virtual void render(Shader &shader) const = 0;

    virtual ~GeometryItem() {
        delete vertexArray;
        delete indexBuffer;
    }

protected:
    VertexArray *vertexArray;
    IndexBuffer *indexBuffer;
};
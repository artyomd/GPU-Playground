//
// Created by Artyom Dangizyan on 11/18/18.
//
#pragma once

#include "Shader.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "TestMVP.h"

namespace test {
    class TestTexture2d : public TestMVP {
    public:
        TestTexture2d();

        ~TestTexture2d() override;

        void onRender() override;

    private:
        Shader *shader;
        VertexArray *vertexArray;
        IndexBuffer *indexBuffer;
    };
}

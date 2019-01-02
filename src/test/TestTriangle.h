//
// Created by Artyom Dangizyan on 2018-11-29.
//
#pragma once

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Triangle.h"
#include "TestMVP.h"

namespace test {
    class TestTriangle : public TestMVP {
    public:
        TestTriangle();

        ~TestTriangle() override;

        void onRender() override;

    private:
        Shader *shader;
        Triangle *triangle;
    };
}

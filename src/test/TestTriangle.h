//
// Created by Artyom Dangizyan on 2018-11-29.
//
#pragma once

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Triangle.h"
#include "MVPTest.h"

namespace test {
    class TestTriangle : public MVPTest {
    public:
        TestTriangle();

        ~TestTriangle() override;

        void onRender() override;

    private:
        Shader *shader;
        Triangle *triangle;
    };
}

//
// Created by Artyom Dangizyan on 2018-11-29.
//
#pragma once

#include "Test.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Triangle.h"

namespace test {
    class TestTriangle : public Test {
    public:
        TestTriangle();

        ~TestTriangle() override;

        void onRender() override;

    private:
        Shader *m_shader;
        Triangle* m_triangle;
    };
}

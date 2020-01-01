//
// Created by Artyom Dangizyan on 2018-11-29.
//
#pragma once

#include "api/gl/GlVertexBinding.h"
#include "api/IndexBuffer.h"
#include "api/Shader.h"
#include "Triangle.h"
#include "TestMVP.h"

namespace test {
    class TestTriangle : public TestMVP {
    public:
        TestTriangle(api::Renderer *renderer);

        ~TestTriangle() override;

        void onRender() override;

    private:
        Shader *shader;
        geometry::Triangle *triangle;
    };
}

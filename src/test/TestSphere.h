//
// Created by Artyom Dangizyan on 2018-11-30.
//
#pragma once

#include "../Shader.h"
#include "../VertexArray.h"
#include "../IndexBuffer.h"
#include "../geometry/StackedSphere.h"
#include "../geometry/SpiralSphere.h"
#include "MVPTest.h"

namespace test {
    class TestSphere : public MVPTest {
    public:
        TestSphere();

        ~TestSphere() override;

        void onRender() override;

    private:
        Shader *shader;
        GeometryItem *sphere;
    };
}

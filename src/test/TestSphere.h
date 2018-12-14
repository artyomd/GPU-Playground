//
// Created by Artyom Dangizyan on 2018-11-30.
//
#pragma once

#include "Test.h"
#include "../Shader.h"
#include "../VertexArray.h"
#include "../IndexBuffer.h"
#include "../geometry/StackedSphere.h"
#include "../geometry/SpiralSphere.h"


namespace test {
    class TestSphere : public Test {
    public:
        TestSphere();

        ~TestSphere() override;

        void onRender() override;

    private:
        Shader *m_shader;
        SpiralSphere *sphere;
        glm::mat4 projection;
        float rotation = 0;
    };
}

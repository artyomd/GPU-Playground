//
// Created by artyomd on 12/30/18.
//

#pragma once


#include <Shader.h>
#include <VertexArray.h>
#include <IndexBuffer.h>
#include "Test.h"

namespace test {
    class MVPTest : public Test {
    public:
        MVPTest();

        void onImGuiRender() override;

        glm::mat4 computeMVP();

        void onWindowSizeChanged(int width, int height) override;

    private:
        glm::mat4 projection;
        glm::vec3 translation;
        glm::vec3 scale;
        glm::vec3 rotate;
        float maxSide;
    };
}

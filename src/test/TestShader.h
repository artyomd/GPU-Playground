//
// Created by artyomd on 1/2/19.
//

#pragma once

#include <geometry/Quad.h>
#include <Shader.h>
#include "Test.h"

namespace test {
    class TestShader : public Test {
    protected:
        TestShader(std::string vertexShaderName, std::string fragmentShaderName);

    public:
        ~TestShader() override;

        void onRender() override;

        void onWindowSizeChanged(int width, int height) override;

    private:
        Quad *quad;
        Shader *shader;
    };
};

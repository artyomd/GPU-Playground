//
// Created by artyomd on 1/2/19.
//

#pragma once

#include <geometry/Quad.h>
#include <Shader.h>
#include <ShaderPropertyFloat1.h>
#include "Test.h"

namespace test {
    class TestShader : public Test {
    protected:
        TestShader(const std::string &vertexShaderName, const std::string &fragmentShaderName);

    public:
        ~TestShader() override;

        void onRender() override;

        void onWindowSizeChanged(int width, int height) override;

        void onImGuiRender() override;

        void onUpdate(float deltaTime) override;

    private:
        Quad *quad;
        Shader *shader;
        ShaderPropertyFloat1* timeUniform;
    };
}

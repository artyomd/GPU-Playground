//
// Created by artyomd on 12/6/19.
//

#pragma once

#include <GLFW/glfw3.h>
#include <cstdio>
#include <test/Test.h>
#include "TestApplication.h"
#include "Renderer.hpp"

namespace application {
    class GLFWApplication : public TestApplication {
    private:
        const float deltaTime = 0.03333333333f;
    protected:
        GLFWwindow *window = nullptr;
        int windowWidth = 640;
        int windowHeight = 480;

        virtual void setupWindowHints() = 0;

        virtual void onWindowSizeChanged() = 0;

        virtual bool prepareFrame() = 0;

        virtual void createImGuiFrame() = 0;

        virtual void renderImGui() = 0;

        virtual void drawFrame() = 0;

        virtual void prepareForShutdown() = 0;

    public:

        GLFWApplication();

        ~GLFWApplication() override;

        void initWindow() final;

        void run() final;

        void destroyWindow() final;
    };
}
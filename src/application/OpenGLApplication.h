//
// Created by artyomd on 12/6/19.
//

#pragma once

#include "GLFWApplication.h"

namespace application {
    class OpenGLApplication : public GLFWApplication {
    protected:
        void setupWindowHints() final;

        void onWindowSizeChanged() final;

        bool prepareFrame() final;

        void createImGuiFrame() final;

        void renderImGui() final;

        void drawFrame() final;

        void prepareForShutdown() final;

    public:
        void initContext() final;

        void initImGui() final;

        void destroyImGui() final;

        void destroyContext() override;
    };
}
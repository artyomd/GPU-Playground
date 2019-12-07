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

        void initImGui() final;

        void attachContext() final;

        void clear() final;

        void createImGuiFrame() final;

        void renderImGui() final;

        void swapBuffers() final;

        void deAttachContext() final;

        void destroyImGui() final;

    public:
        void initContext() final;

        void destroyContext() override;
    };
}
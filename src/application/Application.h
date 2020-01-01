//
// Created by artyomd on 12/6/19.
//

#pragma once

#include <string>

namespace application {
    class Application {
    protected:
        api::Renderer *renderer = nullptr;
    public:
        virtual void initWindow() = 0;

        virtual void initContext() = 0;

        virtual void initImGui() = 0;

        virtual void run() = 0;

        virtual void destroyImGui() = 0;

        virtual void destroyContext() = 0;

        virtual void destroyWindow() = 0;
    };
}
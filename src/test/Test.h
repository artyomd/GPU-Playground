//
// Created by Artyom Dangizyan on 11/17/18.
//
#pragma once

#include "Renderer.hpp"

namespace test {
    class Test {
    protected:
        api::Renderer *renderer;
    public:
        Test(api::Renderer *renderer) : renderer(renderer) {};

        virtual ~Test() = default;

        virtual void onClear() {}

        virtual void onUpdate(float deltaTime) {};

        virtual void onRender() {};

        virtual void onImGuiRender() {};

        virtual void onWindowSizeChanged(int width, int height) {};
    };
}

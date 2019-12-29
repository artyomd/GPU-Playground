//
// Created by Artyom Dangizyan on 11/17/18.
//
#pragma once

#include "RenderingContext.hpp"

namespace test {
    class Test {
    public:
        Test() = default;

        virtual ~Test() = default;

        virtual void onClear(RenderingContext* context) {}

        virtual void onUpdate(float deltaTime) {};

        virtual void onRender() {};

        virtual void onImGuiRender() {};

        virtual void onWindowSizeChanged(int width, int height) {};
    };
}

//
// Created by Artyom Dangizyan on 11/17/18.
//
#pragma once

namespace test {
    class Test {
    public:
        Test() = default;

        virtual ~Test() = default;

        virtual void onUpdate(float deltaTime) {};

        virtual void onRender() {};

        virtual void onImGuiRender() {};
    };
}

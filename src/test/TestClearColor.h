//
// Created by Artyom Dangizyan on 11/17/18.
//
#pragma once

#include "Test.h"

namespace test {
    class TestClearColor : public Test {
    public:
        TestClearColor();

        ~TestClearColor() override = default;

        void onRender() override;

        void onImGuiRender() override;

    private:
        float m_Color[4];
    };
}
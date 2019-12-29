//
// Created by Artyom Dangizyan on 11/17/18.
//
#pragma once

#include "Test.h"

namespace test {
    class TestClearColor : public Test {
    public:
        TestClearColor();

        void onClear(RenderingContext* context) override;

        void onImGuiRender() override;

    private:
        float m_Color[4];
    };
}
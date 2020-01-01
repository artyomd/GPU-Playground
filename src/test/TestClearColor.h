//
// Created by Artyom Dangizyan on 11/17/18.
//
#pragma once

#include "Test.h"

namespace test {
    class TestClearColor : public Test {
    public:
        void onClear() override;

        void onImGuiRender() override;

    private:
        float m_Color[4] = {0.2f, 0.3f, 0.8f, 1.0f};
    };
}
//
// Created by Artyom Dangizyan on 11/18/18.
//
#pragma once

#include <vector>
#include <functional>
#include <iostream>
#include <string>
#include "Test.h"

namespace test {
    class TestMenu : public Test {
    public:
        explicit TestMenu(api::Renderer *renderer, Test *&currentTest);

        void onImGuiRender() override;

        void onWindowSizeChanged(int width, int height) override;

        template<typename T>
        void RegisterTest(const std::string &name) {
            std::cout << "Registering test: " << name << std::endl;
            m_Tests.push_back(std::make_pair(name, []() { return new T(); }));
        }

    private:
        int m_width;
        int m_height;
        Test *&m_CurrentTest;
        std::vector<std::pair<std::string, std::function<Test *()>>> m_Tests;
    };
}

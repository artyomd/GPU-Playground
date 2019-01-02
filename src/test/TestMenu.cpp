//
// Created by Artyom Dangizyan on 11/18/18.
//

#include "TestMenu.h"
#include "imgui/imgui.h"

namespace test {
    test::TestMenu::TestMenu(Test *&currentTest) : m_CurrentTest(currentTest) {
    }

    void test::TestMenu::onImGuiRender() {
        for (auto &test : m_Tests) {
            if (ImGui::Button(test.first.c_str())) {
                m_CurrentTest = test.second();
                m_CurrentTest->onWindowSizeChanged(width, height);
            }
        }
    }

    void TestMenu::onWindowSizeChanged(int width, int height) {
        this->width = width;
        this->height = height;
    }
}

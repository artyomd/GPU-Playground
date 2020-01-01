//
// Created by Artyom Dangizyan on 11/18/18.
//

#include "TestMenu.h"
#include "imgui/imgui.h"

namespace test {
    test::TestMenu::TestMenu(api::Renderer *renderer, Test *&currentTest) : Test(renderer),
                                                                            m_CurrentTest(currentTest),
                                                                            m_width(0), m_height(0) {
    }

    void test::TestMenu::onImGuiRender() {
        for (auto &test : m_Tests) {
            if (ImGui::Button(test.first.c_str())) {
                m_CurrentTest = test.second();
                m_CurrentTest->onWindowSizeChanged(m_width, m_height);
            }
        }
    }

    void TestMenu::onWindowSizeChanged(int width, int height) {
        this->m_width = width;
        this->m_height = height;
    }
}

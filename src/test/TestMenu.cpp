//
// Created by Artyom Dangizyan on 11/18/18.
//

#include "TestMenu.h"
#include "imgui/imgui.h"

namespace test {
    test::TestMenu::TestMenu(api::Renderer *renderer, Test *&current_test) : Test(renderer),
                                                                            m_current_test_(current_test),
                                                                            m_width_(0), m_height_(0) {
    }

    void test::TestMenu::OnImGuiRender() {
        for (auto &test : m_tests_) {
            if (ImGui::Button(test.first.c_str())) {
                m_current_test_ = test.second();
              m_current_test_->OnWindowSizeChanged(m_width_, m_height_);
            }
        }
    }

    void TestMenu::OnWindowSizeChanged(int width, int height) {
        this->m_width_ = width;
        this->m_height_ = height;
    }
}

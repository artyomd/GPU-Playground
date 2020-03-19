//
// Created by Artyom Dangizyan on 11/18/18.
//

#include "TestMenu.h"
#include "imgui/imgui.h"

namespace test {
    test::TestMenu::TestMenu(api::Renderer *renderer, Test *&current_test) : Test(renderer),
                                                                             current_test_(current_test){
    }
    void test::TestMenu::OnImGuiRender() {
        for (auto &test : m_tests_) {
            if (ImGui::Button(test.first.c_str())) {
              current_test_ = test.second();
            }
        }
    }
}

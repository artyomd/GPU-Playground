//
// Created by Artyom Dangizyan on 11/18/18.
//

#include "src/test/test_menu.hpp"

#include <imgui/imgui.h>
#include <utility>

test::TestMenu::TestMenu(std::shared_ptr<api::RenderingContext> context,
                         std::function<void(std::shared_ptr<test::Test>)> test_change_listener)
    : Test(std::move(context)), test_change_listener_(std::move(test_change_listener)) {
}
void test::TestMenu::OnImGuiRender() {
  for (auto &test: m_tests_) {
    if (ImGui::Button(test.first.c_str())) {
      test_change_listener_(test.second());
    }
  }
}

//
// Created by artyomd on 12/7/19.
//

#include <vendor/imgui/imgui.h>
#include "TestApplication.h"
#include <iostream>

namespace application {
void TestApplication::PrepareTestMenu() {
  if (test_menu_!=nullptr) {
    throw std::runtime_error("prepare test menu should be could only once");
  }
  test_menu_ = new test::TestMenu(renderer_, current_test_);
  current_test_ = test_menu_;
}

void TestApplication::RenderMenu() {
  return_pressed_ = current_test_!=test_menu_ && ImGui::Button("<--");
}

void TestApplication::PostRender() {
  if (return_pressed_) {
    renderer_->GetRenderingContext()->WaitForGpuIdle();
    delete current_test_;
    current_test_ = test_menu_;
  }
}

void TestApplication::DeleteTestMenu() {
  delete current_test_;
  if (current_test_!=test_menu_) {
    delete test_menu_;
  }
}
}
//
// Created by artyomd on 12/7/19.
//
#include "src/application/test_application.hpp"

#include <iostream>
#include <utility>
#include <imgui/imgui.h>

void application::TestApplication::PrepareTestMenu() {
  if (test_menu_ != nullptr) {
    throw std::runtime_error("prepare test menu should be could only once");
  }
  test_menu_ = std::make_shared<test::TestMenu>(renderer_, [this](std::shared_ptr<test::Test> test) {
    current_test_ = std::move(test);
  });
  current_test_ = test_menu_;
}

void application::TestApplication::RenderMenu() {
  return_pressed_ = current_test_ != test_menu_ && ImGui::Button("<--");
}

void application::TestApplication::PostRender() {
  if (return_pressed_) {
    ResetMenu();
  }
}
void application::TestApplication::ResetMenu() {
  renderer_->GetContext()->WaitForGpuIdle();
  current_test_ = test_menu_;
}

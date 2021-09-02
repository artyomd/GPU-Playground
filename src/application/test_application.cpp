//
// Created by artyomd on 12/7/19.
//
#include "src/application/test_application.hpp"

#include <iostream>
#include <utility>
#include <imgui/imgui.h>

application::TestApplication::TestApplication() = default;

void application::TestApplication::InitTestMenu() {
  test_menu_ = std::make_shared<test::TestMenu>(context_, [this](std::shared_ptr<test::Test> test) {
    current_test_ = std::move(test);
    OnTestChanged(current_test_);
  });
  current_test_ = test_menu_;
  OnTestChanged(current_test_);
}

void application::TestApplication::RenderMenu() {
  if (current_test_ != test_menu_ && ImGui::Button("<--")) {
    current_test_ = test_menu_;
    OnTestChanged(current_test_);
  }
}

const std::shared_ptr<test::Test> &application::TestApplication::GetCurrentTest() const {
  return current_test_;
}
std::shared_ptr<api::RenderingContext> application::TestApplication::GetContext() const {
  return context_;
}
void application::TestApplication::SetContext(const std::shared_ptr<api::RenderingContext> &context) {
  context_ = context;
  if (test_menu_ != nullptr) {
    test_menu_->SetRenderingContext(context);
  }
}
void application::TestApplication::CleanupTestMenu() {
  this->current_test_ = nullptr;
  this->test_menu_ = nullptr;
}


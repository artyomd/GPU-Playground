//
// Created by artyomd on 12/7/19.
//
#pragma once

#include "src/application/application.hpp"
#include "src/test/test.h"
#include "src/test/test_menu.hpp"

namespace application {
class TestApplication : public Application {

  std::shared_ptr<test::TestMenu> test_menu_ = nullptr;
  bool return_pressed_ = false;

 protected:
  std::shared_ptr<test::Test> current_test_ = nullptr;

  explicit TestApplication(const std::shared_ptr<api::RenderingContext> &context);

 protected:

  void PrepareTestMenu();

  virtual void RenderMenu() final;

  void PostRender();

  void ResetMenu();

 public:
  template<typename T>
  void RegisterTest(const std::string &name) {
    if (test_menu_ != nullptr) {
      test_menu_->RegisterTest<T>(name);
    }
  }
};
}

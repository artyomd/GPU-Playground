//
// Created by artyomd on 12/7/19.
//
#pragma once

#include "test/Test.h"
#include "test/TestMenu.h"
#include "Application.h"

namespace application {
class TestApplication : public Application {
 private:
  test::TestMenu *test_menu_ = nullptr;
  bool return_pressed_ = false;
 protected:
  test::Test *current_test_ = nullptr;

  void PrepareTestMenu();

  virtual void RenderMenu() final;

  void PostRender();

  void DeleteTestMenu();
 public:

  template<typename T>
  void RegisterTest(const std::string &name) {
    if (test_menu_!=nullptr) {
      test_menu_->RegisterTest<T>(name);
    }
  }
};
}
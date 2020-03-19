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
 protected:
  test::Test *current_test_ = nullptr;

  void PrepareTestMenu();

  virtual void RenderMenu() final;

  void DeleteTestMenu();
 public:

  template<typename T>
  void RegisterTest(const std::string &name) {
    test_menu_->RegisterTest<T>(name);
  }
};
}
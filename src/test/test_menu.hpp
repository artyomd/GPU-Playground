//
// Created by Artyom Dangizyan on 11/18/18.
//
#pragma once

#include <functional>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "src/test/test.h"

namespace test {
class TestMenu : public Test {
  std::vector<std::pair<std::string, std::function<std::shared_ptr<Test>()>>> m_tests_;
  std::function<void(std::shared_ptr<test::Test>)> test_change_listener_;
 public:
  TestMenu(std::shared_ptr<api::RenderingContext> context,
           std::function<void(std::shared_ptr<test::Test>)> test_change_listener);

  void OnImGuiRender() override;

  void SetRenderingContext(const std::shared_ptr<api::RenderingContext> &context) {
    this->rendering_context_ = context;
  }

  template<typename T>
  void RegisterTest(const std::string &name) {
    std::cout << "Registering test: " << name << std::endl;
    m_tests_.push_back(std::make_pair(name, [this]() {
      return std::make_shared<T>(rendering_context_);
    }));
  }
};
}

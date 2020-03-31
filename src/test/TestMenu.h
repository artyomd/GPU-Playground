//
// Created by Artyom Dangizyan on 11/18/18.
//
#pragma once

#include <vector>
#include <functional>
#include <iostream>
#include <string>
#include "Test.h"

namespace test {
class TestMenu : public Test {
 public:
  explicit TestMenu(api::Renderer *renderer, Test *&current_test);

  void OnImGuiRender() override;

  template<typename T>
  void RegisterTest(const std::string &name) {
    std::cout << "Registering test: " << name << std::endl;
    m_tests_.push_back(std::make_pair(name, [this]() { return new T(renderer_); }));
  }

 private:
  Test *&current_test_;
  std::vector<std::pair<std::string, std::function<Test *()>>> m_tests_;
};
}

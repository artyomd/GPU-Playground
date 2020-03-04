//
// Created by Artyom Dangizyan on 11/17/18.
//
#pragma once

#include "Test.h"

namespace test {
class TestClearColor : public Test {
 public:
  explicit TestClearColor(api::Renderer *renderer);

  void OnClear() override;

  void OnImGuiRender() override;

  ~TestClearColor() override = default;
 private:
  float color_[4] = {0.2f, 0.3f, 0.8f, 1.0f};
};
}
//
// Created by Artyom Dangizyan on 11/17/18.
//
#pragma once

#include "src/test/Test.h"

#include "src/api/DataType.h"
namespace test {
class TestClearColor : public Test {
 public:
  explicit TestClearColor(std::shared_ptr<api::Renderer> renderer);

  void OnClear() override;

  void OnImGuiRender() override;

  ~TestClearColor() override;
 private:
  api::Pixel_RGBA color_ = {0.2f, 0.3f, 0.8f, 1.0f};
};
}
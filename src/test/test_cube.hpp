//
// Created by artyomd on 5/23/20.
//
#pragma once

#include "src/api/shader.hpp"
#include "src/test/test_model.hpp"

namespace test {
class TestCube : public TestModel {
 public:
  explicit TestCube(std::shared_ptr<api::RenderingContext> rendering_context);

  void OnRender() override;
};

}  // namespace test

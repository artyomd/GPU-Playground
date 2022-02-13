//
// Created by Artyom Dangizyan on 2018-11-29.
//
#pragma once

#include "src/geometry/triangle.hpp"
#include "src/test/test_model.hpp"

namespace test {
class TestTriangle : public TestModel {
 public:
  explicit TestTriangle(
      std::shared_ptr<api::RenderingContext> rendering_context);

  void OnRender() override;
};
}  // namespace test

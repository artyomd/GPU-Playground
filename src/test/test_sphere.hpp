//
// Created by artyomd on 3/23/20.
//
#pragma once

#include "src/geometry/geometry_item.hpp"
#include "src/test/test_model.hpp"

namespace test {
class TestSphere : public TestModel {
 public:
  explicit TestSphere(std::shared_ptr<api::RenderingContext> rendering_context);

  void OnRender() override;
};
}  // namespace test

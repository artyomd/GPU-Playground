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

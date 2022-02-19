#pragma once

#include "src/geometry/quad.hpp"
#include "src/geometry/triangle.hpp"
#include "src/test/test_model.hpp"

namespace test {
class TestTexture2D : public TestModel {
 public:
  explicit TestTexture2D(
      std::shared_ptr<api::RenderingContext> rendering_context);

  void OnRender() override;
};
}  // namespace test

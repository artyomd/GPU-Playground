#pragma once

#include "src/geometry/triangle.hpp"
#include "src/test/test.h"
#include "src/test/test_model.hpp"

namespace test {
class TestObj : public TestModel {
 public:
  explicit TestObj(std::shared_ptr<api::RenderingContext> renderer);

  void OnRender() override;
};
}  // namespace test

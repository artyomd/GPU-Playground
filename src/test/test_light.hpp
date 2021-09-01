//
// Created by artyomd on 5/1/20.
//

#pragma once

#include "src/geometry/triangle.hpp"
#include "src/test/test.h"
#include "src/test/test_model.hpp"

namespace test {
struct ViewBuffer {
  glm::vec4 eye;
  glm::vec4 light;
};
class TestLight : public TestModel {
 public:
  explicit TestLight(std::shared_ptr<api::RenderingContext> renderer);

  void OnRender() override;
 private:
  ViewBuffer buffer_{};
};
}

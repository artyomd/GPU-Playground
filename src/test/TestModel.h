//
// Created by artyomd on 3/8/20.
//

#pragma once

#include "Test.h"

namespace test {
class TestModel : public Test {
 private:
  glm::vec3 translation_;
  glm::vec3 scale_;
  glm::vec3 rotate_;
  float max_side_ = 2.0f;

 protected:
  glm::mat4 ComputeModelMatrix();

 public:
  explicit TestModel(api::Renderer *renderer);

  void OnImGuiRender() override;
};
}
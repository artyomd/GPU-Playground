//
// Created by Artyom Dangizyan on 2018-11-29.
//
#pragma once

#include "api/Renderer.hpp"
#include "geometry/Triangle.h"
#include "Test.h"

namespace test {
class TestTriangle : public Test {
 private:
  api::Shader *vertex_shader_ = nullptr;
  api::Shader *fragment_shader_ = nullptr;
  api::RenderingPipeline *pipeline_ = nullptr;
  geometry::Triangle *triangle_ = nullptr;
 public:
  TestTriangle(api::Renderer *renderer);

  void OnClear() override;

  void OnRender() override;

  ~TestTriangle() override;
};
}

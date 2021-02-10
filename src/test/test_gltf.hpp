//
// Created by artyomd on 12/21/20.
//
#pragma once

#include "src/test/test_model.hpp"
#include "src/geometry/gltf_model.hpp"

namespace test {
class TestGltf : public TestModel {
 private:
  geometry::GltfModel model_;
 public:
  explicit TestGltf(std::shared_ptr<api::RenderingContext> rendering_context);
  void OnViewportChange(size_t width, size_t height) override;
  void OnRender() override;
};
}

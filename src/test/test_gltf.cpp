//
// Created by artyomd on 12/21/20.
//

#include "src/test/test_gltf.hpp"

#include <utility>

#include "src/geometry/gltf_model.hpp"

test::TestGltf::TestGltf(std::shared_ptr<api::RenderingContext> rendering_context) :
    TestModel(std::move(rendering_context)),
    model_(geometry::GltfModel(rendering_context_, "../res/models/Duck.gltf")) {
  model_.LoadScene();
}

void test::TestGltf::OnRender() {
  model_.Render();
}

void test::TestGltf::OnViewportChange(size_t width, size_t height) {
  TestModel::OnViewportChange(width, height);
  model_.SetViewport(width, height);
}


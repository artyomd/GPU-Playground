//
// Created by artyomd on 12/21/20.
//

#include "src/test/test_gltf.hpp"

#include <utility>

test::TestGltf::TestGltf(std::shared_ptr<api::RenderingContext> rendering_context) :
    TestModel(std::move(rendering_context)) {

}
void test::TestGltf::OnRender() {
  Test::OnRender();
}

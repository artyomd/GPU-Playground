#include "src/test/test_triangle.hpp"

#include <utility>

#include "src/geometry/point.hpp"
#include "src/geometry/triangle.hpp"
#include "src/shaders/shaders.hpp"

test::TestTriangle::TestTriangle(
    std::shared_ptr<api::RenderingContext> rendering_context)
    : TestModel(std::move(rendering_context)) {
  geometry::Point point_0 = {-0.5F, -0.5F, 0.0F, 1.0F, 0.0F, 0.0F, 1.0F};
  geometry::Point point_1 = {0.5F, -0.5F, 0.0F, 0.0F, 1.0F, 0.0F, 1.0F};
  geometry::Point point_2 = {0.0F, 0.5F, 0.0F, 0.0F, 0.0F, 1.0F, 1.0F};

  auto triangle = std::make_shared<geometry::Triangle>(
      rendering_context_, point_0, point_1, point_2);
  auto vertex_shader = rendering_context_->CreateShader(
      default_mvp_color_vertex_shader, "main", api::ShaderType::VERTEX);
  auto fragment_shader = rendering_context_->CreateShader(
      default_color_fragment_shader, "main", api::ShaderType::FRAGMENT);
  pipeline_ = rendering_context_->CreateGraphicsPipeline(
      vertex_shader, fragment_shader, triangle->GetVbl(),
      {api::DrawMode::TRIANGLE_LIST, api::CullMode::NONE, api::FrontFace::CW,
       false, api::CompareOp::LESS});
  pipeline_->SetVertexBuffer(triangle->GetVertexBuffer());
  pipeline_->SetIndexBuffer(triangle->GetIndexBuffer(),
                            triangle->GetIndexBufferDataType());
  index_count_ = triangle->GetIndexCount();
}

void test::TestTriangle::OnRender() {
  ubo_.model = ComputeModelMatrix();
  ubo_.proj = orthographic_projection_;
  ubo_.view = glm::mat4(1.0F);
  pipeline_->UpdateUniformBuffer(0, &ubo_);
  pipeline_->Draw(index_count_, 0);
}

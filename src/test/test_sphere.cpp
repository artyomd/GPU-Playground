//
// Created by artyomd on 3/23/20.
//
#include "src/test/test_sphere.hpp"

#include <utility>

#include "src/geometry/stacked_sphere.hpp"
#include "src/geometry/spiral_sphere.hpp"
#include "src/shaders/shaders.hpp"

test::TestSphere::TestSphere(std::shared_ptr<api::RenderingContext> rendering_context) : TestModel(std::move(
    rendering_context)) {
  auto sphere = std::make_shared<geometry::SpiralSphere>(rendering_context_, 1.0F, 32, 64);
  auto vertex_shader = rendering_context_->CreateShader(default_mvp_color_vertex_shader,
                                                        "main",
                                                        api::ShaderType::VERTEX);
  auto fragment_shader = rendering_context_->CreateShader(default_color_fragment_shader,
                                                          "main",
                                                          api::ShaderType::FRAGMENT);
  pipeline_ = rendering_context_->CreateGraphicsPipeline(vertex_shader,
                                                         fragment_shader,
                                                         sphere->GetVbl(),
                                                         {
                                                             api::DrawMode::TRIANGLE_STRIP,
                                                             api::CullMode::NONE,
                                                             api::FrontFace::CCW,
                                                             true,
                                                             api::CompareOp::LESS});
  pipeline_->SetVertexBuffer(sphere->GetVertexBuffer());
  pipeline_->SetIndexBuffer(sphere->GetIndexBuffer(), sphere->GetIndexBufferDataType());
  index_count_ = sphere->GetIndexCount();
}
void test::TestSphere::OnRender() {
  ubo_.model = ComputeModelMatrix();
  ubo_.proj = orthographic_projection_;
  ubo_.view = glm::mat4(1.0F);
  pipeline_->UpdateUniformBuffer(0, &ubo_);
  pipeline_->Draw(index_count_, 0);
}

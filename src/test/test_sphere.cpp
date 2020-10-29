//
// Created by artyomd on 3/23/20.
//
#include "src/test/test_sphere.hpp"

#include <utility>

#include "src/geometry/stacked_sphere.hpp"
#include "src/geometry/spiral_sphere.hpp"

test::TestSphere::TestSphere(std::shared_ptr<api::RenderingContext> rendering_context) : TestModel(std::move(
    rendering_context)) {
  sphere_ = std::make_shared<geometry::SpiralSphere>(rendering_context_, 1.0f, 32, 64);
  vertex_shader_ = rendering_context_->CreateShader("../res/shader/compiled/default_mvp_color_vertex_shader.spv",
                                                    "main",
                                                    api::ShaderType::SHADER_TYPE_VERTEX);
  fragment_shader_ = rendering_context_->CreateShader("../res/shader/compiled/default_color_fragment_shader.spv",
                                                      "main",
                                                      api::ShaderType::SHADER_TYPE_FRAGMENT);
  pipeline_ = rendering_context_->CreateGraphicsPipeline(sphere_->GetVertexBuffer(),
                                                         sphere_->GetIndexBuffer(),
                                                         vertex_shader_, fragment_shader_,
                                                         {
                                                             api::DrawMode::TRIANGLE_STRIP,
                                                             api::CullMode::NONE,
                                                             api::FrontFace::CCW,
                                                             true,
                                                             api::CompareOp::LESS});

  uniform_buffer_ = rendering_context_->CreateUniformBuffer(sizeof(UniformBufferObjectMvp),
                                                            0,
                                                            api::ShaderType::SHADER_TYPE_VERTEX);
  pipeline_->AddUniform(uniform_buffer_);
}
void test::TestSphere::OnRender() {
  ubo_->model = ComputeModelMatrix();
  ubo_->proj = orthographic_projection_;
  ubo_->view = glm::mat4(1.0f);
  uniform_buffer_->Update(ubo_.get());
  pipeline_->Render();
}

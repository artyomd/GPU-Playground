//
// Created by Artyom Dangizyan on 2018-11-29.
//
#include "src/test/test_triangle.hpp"

#include <utility>

#include "src/geometry/point.hpp"
#include "src/geometry/triangle.hpp"

test::TestTriangle::TestTriangle(std::shared_ptr<api::RenderingContext> rendering_context)
    : TestModel(std::move(rendering_context)) {

  geometry::Point point_0 = {-0.5f, -0.5f, 0.0f, 1, 0, 0, 1};
  geometry::Point point_1 = {0.5f, -0.5f, 0.0f, 0, 1, 0, 1};
  geometry::Point point_2 = {0.0f, 0.5f, 0.0f, 0, 0, 1, 1};

  triangle_ = std::make_shared<geometry::Triangle>(rendering_context_, point_0, point_1, point_2);
  vertex_shader_ = rendering_context_->CreateShader("../res/shader/compiled/default_mvp_color_vertex_shader.spv",
                                                    "main",
                                                    api::ShaderType::SHADER_TYPE_VERTEX);
  fragment_shader_ = rendering_context_->CreateShader("../res/shader/compiled/default_color_fragment_shader.spv",
                                                      "main",
                                                      api::ShaderType::SHADER_TYPE_FRAGMENT);
  pipeline_ = rendering_context_->CreateGraphicsPipeline(triangle_->GetVertexBuffer(),
                                                         triangle_->GetIndexBuffer(),
                                                         vertex_shader_,
                                                         fragment_shader_,
                                                         {
                                                             api::DrawMode::TRIANGLE_LIST,
                                                             api::CullMode::NONE,
                                                             api::FrontFace::CW,
                                                             false,
                                                             api::CompareOp::LESS
                                                         });

  uniform_buffer_ = rendering_context_->CreateUniformBuffer(sizeof(UniformBufferObjectMvp),
                                                            0,
                                                            api::ShaderType::SHADER_TYPE_VERTEX);
  pipeline_->AddUniform(uniform_buffer_);
  pipeline_->ViewportChanged(size_[0], size_[1]);
}

void test::TestTriangle::OnRender() {
  ubo_->model = ComputeModelMatrix();
  ubo_->proj = orthographic_projection_;
  ubo_->view = glm::mat4(1.0f);
  uniform_buffer_->Update(ubo_.get());
  pipeline_->Render();
}

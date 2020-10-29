//
// Created by artyomd on 3/21/20.
//

#include "src/test/test_shader.hpp"

#include <utility>

#include "src/geometry/point.hpp"
#include "src/geometry/quad.hpp"

test::TestShader::TestShader(std::shared_ptr<api::RenderingContext> rendering_context,
                             std::string fragment_shader)
    : Test(std::move(rendering_context)) {
  geometry::Point point_0 = {-1.0f, 1.0f, 0.0f};
  geometry::Point point_1 = {1.0f, 1.0f, 0.0f};
  geometry::Point point_2 = {1.0f, -1.0f, 0.0f};
  geometry::Point point_3 = {-1.0f, -1.0f, 0.0f};

  quad_ = std::make_shared<geometry::Quad>(rendering_context_, point_0, point_1, point_2, point_3);

  vertex_shader_ = rendering_context_->CreateShader("../res/shader/compiled/default_empty_vertex_shader.spv",
                                                    "main",
                                                    api::ShaderType::SHADER_TYPE_VERTEX);

  fragment_shader_ = rendering_context_->CreateShader(std::move(fragment_shader),
                                                      "main",
                                                      api::ShaderType::SHADER_TYPE_FRAGMENT);

  pipeline_ = rendering_context_->CreateGraphicsPipeline(quad_->GetVertexBuffer(),
                                                         quad_->GetIndexBuffer(),
                                                         vertex_shader_,
                                                         fragment_shader_,
                                                         {
                                                             api::DrawMode::TRIANGLE_STRIP,
                                                             api::CullMode::NONE,
                                                             api::FrontFace::CW,
                                                             false,
                                                             api::CompareOp::LESS
                                                         });

  uniform_buffer_ = rendering_context_->CreateUniformBuffer(sizeof(UniformBufferObjectShader),
                                                            0,
                                                            api::ShaderType::SHADER_TYPE_FRAGMENT);

  pipeline_->AddUniform(uniform_buffer_);
}

void test::TestShader::OnRender() {
  uniform_buffer_->Update(ubo_.get());
  pipeline_->Render();
}

void test::TestShader::OnUpdate(float delta_time) {
  ubo_->time += delta_time;
}

void test::TestShader::OnImGuiRender() {
//  if (ImGui::Button("Reload")) {
//    shader->recompile();
//  }
}

void test::TestShader::OnViewportChange(size_t width, size_t height) {
  Test::OnViewportChange(width, height);
  ubo_->screen_height = (float) height;
  ubo_->screen_width = (float) width;
}


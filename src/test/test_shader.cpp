//
// Created by artyomd on 3/21/20.
//

#include "src/test/test_shader.hpp"

#include <utility>

#include "src/geometry/point.hpp"
#include "src/geometry/quad.hpp"

test::TestShader::TestShader(std::shared_ptr<api::Renderer> renderer,
                             std::shared_ptr<api::Shader> fragment_shader)
    : Test(std::move(renderer)), fragment_shader_(std::move(fragment_shader)) {
  geometry::Point point_0 = {-1.0f, 1.0f, 0.0f};
  geometry::Point point_1 = {1.0f, 1.0f, 0.0f};
  geometry::Point point_2 = {1.0f, -1.0f, 0.0f};
  geometry::Point point_3 = {-1.0f, -1.0f, 0.0f};

  auto context = renderer_->GetContext();

  uniform_buffer_ = context->CreateUniformBuffer(sizeof(UniformBufferObjectShader),
                                                 0,
                                                 api::ShaderType::SHADER_TYPE_FRAGMENT);
  std::vector<std::shared_ptr<api::Uniform>> uniforms;
  uniforms.push_back(uniform_buffer_);
  pipeline_layout_ = context->CreateRenderingPipelineLayout(uniforms);

  quad_ = std::make_shared<geometry::Quad>(context, point_0, point_1, point_2, point_3);

  vertex_shader_ = context->CreateShader("../res/shader/compiled/default_empty_vertex_shader.spv",
                                         "../res/shader/default_empty_vertex_shader.glsl",
                                         "main",
                                         api::ShaderType::SHADER_TYPE_VERTEX);

  pipeline_ = context->CreateGraphicsPipeline(quad_->GetVertexBinding(),
                                              quad_->GetIndexBuffer(),
                                              vertex_shader_,
                                              fragment_shader_,
                                              pipeline_layout_,
                                              {});
  UpdateUniformBufferScreenSize();
}

void test::TestShader::OnRender() {
  uniform_buffer_->Update(ubo_.get());
  api::Renderer::Render(pipeline_);
}

void test::TestShader::OnUpdate(float delta_time) {
  ubo_->time += delta_time;
}

void test::TestShader::OnImGuiRender() {
//  if (ImGui::Button("Reload")) {
//    shader->recompile();
//  }
}

void test::TestShader::OnViewportChange() {
  pipeline_->ViewportChanged();
  UpdateUniformBufferScreenSize();
}

void test::TestShader::UpdateUniformBufferScreenSize() {
  auto size = renderer_->GetViewportSize();
  ubo_->screen_height = (float) size.height;
  ubo_->screen_width = (float) size.width;
}


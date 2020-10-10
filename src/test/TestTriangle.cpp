//
// Created by Artyom Dangizyan on 2018-11-29.
//
#include "src/test/TestTriangle.h"

#include <utility>

#include "src/geometry/Point.hpp"
#include "src/geometry/Triangle.h"

namespace test {

TestTriangle::TestTriangle(std::shared_ptr<api::Renderer> renderer) : TestModel(std::move(renderer)) {

  geometry::Point point_0 = {-0.5f, -0.5f, 0.0f, 1, 0, 0, 1};
  geometry::Point point_1 = {0.5f, -0.5f, 0.0f, 0, 1, 0, 1};
  geometry::Point point_2 = {0.0f, 0.5f, 0.0f, 0, 0, 1, 1};

  auto context = renderer_->GetContext();

  uniform_buffer_ = context->CreateUniformBuffer(sizeof(UniformBufferObjectMvp),
                                                 0,
                                                 api::ShaderType::SHADER_TYPE_VERTEX);
  std::vector<std::shared_ptr<api::Uniform>> uniforms;
  uniforms.push_back(uniform_buffer_);
  pipeline_layout_ = context->CreateRenderingPipelineLayout(uniforms);

  triangle_ = std::make_shared<geometry::Triangle>(context, point_0, point_1, point_2);
  vertex_shader_ = context->CreateShader("../res/shader/compiled/default_mvp_color_vertex_shader.spv",
                                         "../res/shader/default_mvp_color_vertex_shader.glsl",
                                         "main",
                                         api::ShaderType::SHADER_TYPE_VERTEX);
  fragment_shader_ = context->CreateShader("../res/shader/compiled/default_color_fragment_shader.spv",
                                           "../res/shader/default_color_fragment_shader.glsl",
                                           "main",
                                           api::ShaderType::SHADER_TYPE_FRAGMENT);
  pipeline_ = context->CreateGraphicsPipeline(triangle_->GetVertexBinding(), triangle_->GetIndexBuffer(),
                                              vertex_shader_, fragment_shader_, pipeline_layout_,
                                              {});
}

void TestTriangle::OnClear() {
  renderer_->SetClearColor({0.0f, 0.0f, 0.0f, 1.0f});
}

void TestTriangle::OnRender() {
  ubo_->model = ComputeModelMatrix();
  ubo_->proj = renderer_->GetContext()->GetOrthographicProjection();
  ubo_->view = glm::mat4(1.0f);
  uniform_buffer_->Update(ubo_.get());
  api::Renderer::Render(pipeline_);
}

void TestTriangle::OnViewportChange() {
  pipeline_->ViewportChanged();
}
}
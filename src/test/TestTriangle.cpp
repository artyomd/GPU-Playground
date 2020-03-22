//
// Created by Artyom Dangizyan on 2018-11-29.
//

#include <iostream>
#include "geometry/Point.hpp"
#include "geometry/Triangle.h"
#include "TestTriangle.h"

namespace test {

TestTriangle::TestTriangle(api::Renderer *renderer) : TestModel(renderer) {

  geometry::Point point_0 = {-0.5f, -0.5f, 0.0f, 1, 1, 0, 0};
  geometry::Point point_1 = {0.5f, -0.5f, 0.0f, 1, 0, 1, 0};
  geometry::Point point_2 = {0.0f, 0.5f, 0.0f, 1, 0, 0, 1};

  auto *context = renderer->GetRenderingContext();

  uniform_buffer_ = context->CreateUniformBuffer(sizeof(UniformBufferObjectMvp),
                                                 0,
                                                 api::ShaderType::SHADER_TYPE_VERTEX);

  triangle_ = new geometry::Triangle(context, point_0, point_1, point_2);
  vertex_shader_ = context->CreateShader("../res/shader/compiled/default_mvp_color_vertex_shader.spv",
                                         "../res/shader/default_mvp_color_vertex_shader.glsl",
                                         "main",
                                         api::SHADER_TYPE_VERTEX);
  fragment_shader_ = context->CreateShader("../res/shader/compiled/default_color_fragment_shader.spv",
                                           "../res/shader/default_color_fragment_shader.glsl",
                                           "main",
                                           api::SHADER_TYPE_FRAGMENT);
  pipeline_ = context->CreateGraphicsPipeline(triangle_->GetVertexBinding(), triangle_->GetIndexBuffer(),
                                              vertex_shader_, fragment_shader_, uniform_buffer_);
}

void TestTriangle::OnClear() {
  renderer_->SetClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void TestTriangle::OnRender() {
  ubo_->model_ = ComputeModelMatrix();
  ubo_->proj_ = renderer_->GetRenderingContext()->GetOrthoProjection();
  ubo_->view_ = glm::mat4(1.0f);
  uniform_buffer_->Update(ubo_);
  api::Renderer::Render(pipeline_);
}

void TestTriangle::OnViewportChange() {
  pipeline_->ViewportChanged();
}

TestTriangle::~TestTriangle() {
  auto context = renderer_->GetRenderingContext();
  context->FreeGraphicsPipeline(pipeline_);
  context->DeleteShader(fragment_shader_);
  context->DeleteShader(vertex_shader_);
  context->DeleteUniformBuffer(uniform_buffer_);
  delete triangle_;
}
}
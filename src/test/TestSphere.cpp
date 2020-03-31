//
// Created by artyomd on 3/23/20.
//

#include <geometry/StackedSphere.h>
#include <geometry/SpiralSphere.h>
#include "TestSphere.h"

test::TestSphere::TestSphere(api::Renderer *renderer) : TestModel(renderer) {
  auto *context = renderer->GetRenderingContext();

  uniform_buffer_ = context->CreateUniformBuffer(sizeof(UniformBufferObjectMvp),
                                                 0,
                                                 api::ShaderType::SHADER_TYPE_VERTEX);
  std::vector<api::Uniform *> uniforms;
  uniforms.push_back(uniform_buffer_);
  pipeline_layout_ = context->CreateRenderingPipelineLayout(uniforms);

  sphere_ = new geometry::StackedSphere(context, 1.0f);
  vertex_shader_ = context->CreateShader("../res/shader/compiled/default_mvp_color_vertex_shader.spv",
                                         "../res/shader/default_mvp_color_vertex_shader.glsl",
                                         "main",
                                         api::SHADER_TYPE_VERTEX);
  fragment_shader_ = context->CreateShader("../res/shader/compiled/default_color_fragment_shader.spv",
                                           "../res/shader/default_color_fragment_shader.glsl",
                                           "main",
                                           api::SHADER_TYPE_FRAGMENT);
  pipeline_ = context->CreateGraphicsPipeline(sphere_->GetVertexBinding(), sphere_->GetIndexBuffer(),
                                              vertex_shader_, fragment_shader_, pipeline_layout_);
}
void test::TestSphere::OnClear() {
  renderer_->SetClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}
void test::TestSphere::OnRender() {
  ubo_->model_ = ComputeModelMatrix();
  ubo_->proj_ = renderer_->GetRenderingContext()->GetOrthoProjection();
  ubo_->view_ = glm::mat4(1.0f);
  uniform_buffer_->Update(ubo_);
  api::Renderer::Render(pipeline_);
}
void test::TestSphere::OnViewportChange() {
  pipeline_->ViewportChanged();
}
test::TestSphere::~TestSphere() {
  auto context = renderer_->GetRenderingContext();
  context->FreeGraphicsPipeline(pipeline_);
  context->FreeRenderingPipelineLayout(pipeline_layout_);
  context->DeleteShader(fragment_shader_);
  context->DeleteShader(vertex_shader_);
  context->DeleteUniformBuffer(uniform_buffer_);
  delete sphere_;
}

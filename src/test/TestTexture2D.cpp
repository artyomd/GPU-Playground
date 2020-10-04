//
// Created by artyomd on 3/31/20.
//

#include "src/test/TestTexture2D.h"

namespace test {
TestTexture2D::TestTexture2D(api::Renderer *renderer) : TestModel(renderer) {

  float positions[] = {
      -0.5f, -0.5f, 0.0f, 1.0f,
      0.5f, -0.5f, 1.0f, 1.0f,
      -0.5f, 0.5f, 0.0f, 0.0f,
      0.5f, 0.5f, 1.0f, 0.0f
  };

  unsigned short indices[] = {
      0, 1, 2,
      2, 1, 3
  };

  auto *context = renderer->GetRenderingContext();

  vertex_buffer_ = context->CreateVertexBuffer(positions, 4*5*sizeof(float));
  vertex_buffer_layout_ = new api::VertexBufferLayout();
  vertex_buffer_layout_->Push<float>(2);
  vertex_buffer_layout_->Push<float>(2);

  vertex_binding_ = context->CreateVertexBinding(vertex_buffer_, vertex_buffer_layout_);
  index_buffer_ = context->CreateIndexBuffer(indices, 6, api::DATA_TYPE_UINT_16);

  uniform_buffer_ = context->CreateUniformBuffer(sizeof(UniformBufferObjectMvp),
                                                 0,
                                                 api::ShaderType::SHADER_TYPE_VERTEX);
  texture_2_d_ = context->CreateTexture2D("../res/textures/image.png",
                                          1,
                                          api::ShaderType::SHADER_TYPE_FRAGMENT);
  std::vector<api::Uniform *> uniforms;
  uniforms.push_back(uniform_buffer_);
  uniforms.push_back(texture_2_d_);
  pipeline_layout_ = context->CreateRenderingPipelineLayout(uniforms);

  vertex_shader_ = context->CreateShader("../res/shader/compiled/texture2d_vertex.spv",
                                         "../res/shader/texture2d_vertex.glsl",
                                         "main",
                                         api::SHADER_TYPE_VERTEX);
  fragment_shader_ = context->CreateShader("../res/shader/compiled/texture2d_fragment.spv",
                                           "../res/shader/texture2d_fragment.glsl",
                                           "main",
                                           api::SHADER_TYPE_FRAGMENT);
  pipeline_ = context->CreateGraphicsPipeline(vertex_binding_, index_buffer_,
                                              vertex_shader_, fragment_shader_, pipeline_layout_);
}

void TestTexture2D::OnClear() {
  renderer_->SetClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void TestTexture2D::OnRender() {
  ubo_->model_ = ComputeModelMatrix();
  ubo_->proj_ = renderer_->GetRenderingContext()->GetOrthoProjection();
  ubo_->view_ = glm::mat4(1.0f);
  uniform_buffer_->Update(ubo_);
  api::Renderer::Render(pipeline_);
}

void TestTexture2D::OnViewportChange() {
  pipeline_->ViewportChanged();
}

TestTexture2D::~TestTexture2D() {
  auto context = renderer_->GetRenderingContext();
  context->FreeGraphicsPipeline(pipeline_);
  context->FreeRenderingPipelineLayout(pipeline_layout_);
  context->DeleteShader(fragment_shader_);
  context->DeleteShader(vertex_shader_);
  context->DeleteUniformBuffer(uniform_buffer_);
  context->DeleteTexture2D(texture_2_d_);
  context->FreeIndexBuffer(index_buffer_);
  context->FreeVertexBiding(vertex_binding_);
  delete (vertex_buffer_layout_);
  context->FreeVertexBuffer(vertex_buffer_);
}
}
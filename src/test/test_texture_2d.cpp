//
// Created by artyomd on 3/31/20.
//

#include "src/test/test_texture_2d.hpp"

#include <utility>

test::TestTexture2D::TestTexture2D(std::shared_ptr<api::Renderer> renderer) : TestModel(std::move(renderer)) {

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

  auto context = renderer_->GetContext();

  vertex_buffer_ = context->CreateVertexBuffer(positions, 4 * 4 * sizeof(float));

  vertex_buffer_layout_->Push<float>(2);
  vertex_buffer_layout_->Push<float>(2);

  vertex_binding_ = context->CreateVertexBinding(vertex_buffer_, vertex_buffer_layout_);
  index_buffer_ = context->CreateIndexBuffer(indices, 6, api::DataType::DATA_TYPE_UINT_16);

  uniform_buffer_ = context->CreateUniformBuffer(sizeof(UniformBufferObjectMvp),
                                                 0,
                                                 api::ShaderType::SHADER_TYPE_VERTEX);
  texture_2_d_ = context->CreateTexture2D("../res/textures/image.png",
                                          1,
                                          api::ShaderType::SHADER_TYPE_FRAGMENT);
  std::vector<std::shared_ptr<api::Uniform>> uniforms;
  uniforms.push_back(uniform_buffer_);
  uniforms.push_back(texture_2_d_);
  pipeline_layout_ = context->CreateRenderingPipelineLayout(uniforms);

  vertex_shader_ = context->CreateShader("../res/shader/compiled/texture2d_vertex.spv",
                                         "../res/shader/texture2d_vertex.glsl",
                                         "main",
                                         api::ShaderType::SHADER_TYPE_VERTEX);
  fragment_shader_ = context->CreateShader("../res/shader/compiled/texture2d_fragment.spv",
                                           "../res/shader/texture2d_fragment.glsl",
                                           "main",
                                           api::ShaderType::SHADER_TYPE_FRAGMENT);
  pipeline_ = context->CreateGraphicsPipeline(vertex_binding_, index_buffer_,
                                              vertex_shader_, fragment_shader_, pipeline_layout_,
                                              {});
}

void test::TestTexture2D::OnClear() {
  renderer_->SetClearColor({0.0f, 0.0f, 0.0f, 1.0f});
}

void test::TestTexture2D::OnRender() {
  ubo_->model = ComputeModelMatrix();
  ubo_->proj = renderer_->GetContext()->GetOrthographicProjection();
  ubo_->view = glm::mat4(1.0f);
  uniform_buffer_->Update(ubo_.get());
  api::Renderer::Render(pipeline_);
}

void test::TestTexture2D::OnViewportChange() {
  pipeline_->ViewportChanged();
}

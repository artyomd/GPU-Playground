//
// Created by artyomd on 3/31/20.
//

#include "src/test/test_texture_2d.hpp"

#include <utility>

test::TestTexture2D::TestTexture2D(std::shared_ptr<api::RenderingContext> rendering_context)
    : TestModel(std::move(rendering_context)) {

  float positions[] = {
      -0.5f, -0.5f, 0.0f, 0.0f,
      0.5f, -0.5f, 1.0f, 0.0f,
      -0.5f, 0.5f, 0.0f, 1.0f,
      0.5f, 0.5f, 1.0f, 1.0f
  };

  unsigned short indices[] = {
      0, 1, 2,
      2, 1, 3
  };

  api::VertexBufferLayout vertex_buffer_layout;
  vertex_buffer_layout.Push<float>(2);
  vertex_buffer_layout.Push<float>(2);

  vertex_buffer_ = rendering_context_->CreateVertexBuffer(4 * 4 * sizeof(float), vertex_buffer_layout);
  vertex_buffer_->Update(&positions[0]);

  index_buffer_ = rendering_context_->CreateIndexBuffer(6, api::DataType::DATA_TYPE_UINT_16);
  index_buffer_->Update(&indices[0]);

  vertex_shader_ = rendering_context_->CreateShader("../res/shader/compiled/texture2d_vertex.spv",
                                                    "main",
                                                    api::ShaderType::SHADER_TYPE_VERTEX);
  fragment_shader_ = rendering_context_->CreateShader("../res/shader/compiled/texture2d_fragment.spv",
                                                      "main",
                                                      api::ShaderType::SHADER_TYPE_FRAGMENT);
  pipeline_ = rendering_context_->CreateGraphicsPipeline(vertex_buffer_,
                                                         index_buffer_,
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
  texture_2_d_ = rendering_context_->CreateTexture2D("../res/textures/image.png",
                                                     1,
                                                     api::ShaderType::SHADER_TYPE_FRAGMENT);
  texture_2_d_->SetSampler({api::Filter::LINEAR,
                            api::Filter::LINEAR,
                            api::AddressMode::CLAMP_TO_EDGE,
                            api::AddressMode::CLAMP_TO_EDGE,
                            api::AddressMode::CLAMP_TO_EDGE});
  pipeline_->AddUniform(uniform_buffer_);
  pipeline_->AddUniform(texture_2_d_);
}

void test::TestTexture2D::OnRender() {
  ubo_->model = ComputeModelMatrix();
  ubo_->proj = orthographic_projection_;
  ubo_->view = glm::mat4(1.0f);
  uniform_buffer_->Update(ubo_.get());
  pipeline_->Render();
}

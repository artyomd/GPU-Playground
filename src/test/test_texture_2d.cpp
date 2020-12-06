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
  size_t stride = sizeof(float) * 4;
  vertex_buffer_layout.Push({api::DataType::DATA_TYPE_FLOAT, 2, 0, stride});
  vertex_buffer_layout.Push({api::DataType::DATA_TYPE_FLOAT, 2, sizeof(float) * 2, stride});

  auto vertex_buffer = rendering_context_->CreateVertexBuffer(4 * 4 * sizeof(float), vertex_buffer_layout);
  vertex_buffer->Update(&positions[0]);

  auto index_buffer = rendering_context_->CreateIndexBuffer(6, api::DataType::DATA_TYPE_UINT_16);
  index_buffer->Update(&indices[0]);

  auto vertex_shader = rendering_context_->CreateShader("../res/shader/compiled/texture2d_vertex.spv",
                                                        "main",
                                                        api::ShaderType::SHADER_TYPE_VERTEX);
  auto fragment_shader = rendering_context_->CreateShader("../res/shader/compiled/texture2d_fragment.spv",
                                                          "main",
                                                          api::ShaderType::SHADER_TYPE_FRAGMENT);
  auto pipeline = rendering_context_->CreateGraphicsPipeline(vertex_buffer,
                                                             index_buffer,
                                                             vertex_shader,
                                                             fragment_shader,
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
  auto texture_2_d = rendering_context_->CreateTexture2D(1,
                                                         api::ShaderType::SHADER_TYPE_FRAGMENT);
  texture_2_d->Load("../res/textures/image.png");
  texture_2_d->SetSampler({api::Filter::LINEAR,
                           api::Filter::LINEAR,
                           api::AddressMode::CLAMP_TO_EDGE,
                           api::AddressMode::CLAMP_TO_EDGE,
                           api::AddressMode::CLAMP_TO_EDGE});
  pipeline->AddUniform(uniform_buffer_);
  pipeline->AddUniform(texture_2_d);
  pipelines_.push_back(pipeline);
}

void test::TestTexture2D::OnRender() {
  ubo_->model = ComputeModelMatrix();
  ubo_->proj = orthographic_projection_;
  ubo_->view = glm::mat4(1.0f);
  uniform_buffer_->Update(ubo_.get());
  pipelines_[0]->Render();
}

//
// Created by artyomd on 3/31/20.
//

#include "src/test/test_texture_2d.hpp"

#include <stb_image.h>

#include <utility>

#include "src/shaders/shaders.hpp"

test::TestTexture2D::TestTexture2D(
    std::shared_ptr<api::RenderingContext> rendering_context)
    : TestModel(std::move(rendering_context)) {
  float positions[] = {-0.5F, -0.5F, 0.0F, 0.0F, 0.5F, -0.5F, 1.0F, 0.0F,
                       -0.5F, 0.5F,  0.0F, 1.0F, 0.5F, 0.5F,  1.0F, 1.0F};

  unsigned short indices[] = {0, 1, 2, 2, 1, 3};

  api::VertexBufferLayout vertex_buffer_layout;
  vertex_buffer_layout.Push({0, api::DataType::FLOAT, 2});
  vertex_buffer_layout.Push({1, api::DataType::FLOAT, 2});

  auto vertex_buffer = rendering_context_->CreateBuffer(
      4 * 4 * sizeof(float), api::BufferUsage::VERTEX_BUFFER,
      api::MemoryType::DEVICE_LOCAL);
  vertex_buffer->Update(&positions[0]);

  auto index_buffer = rendering_context_->CreateBuffer(
      6 * sizeof(unsigned short), api::BufferUsage::INDEX_BUFFER,
      api::MemoryType::DEVICE_LOCAL);
  index_count_ = 6;
  index_buffer->Update(&indices[0]);

  auto vertex_shader = rendering_context_->CreateShader(
      texture2d_vertex, "main", api::ShaderType::VERTEX);
  auto fragment_shader = rendering_context_->CreateShader(
      texture2d_fragment, "main", api::ShaderType::FRAGMENT);
  pipeline_ = rendering_context_->CreateGraphicsPipeline(
      vertex_shader, fragment_shader, vertex_buffer_layout,
      {api::DrawMode::TRIANGLE_LIST, api::CullMode::NONE, api::FrontFace::CCW,
       false, api::CompareOp::LESS});
  pipeline_->SetVertexBuffer(vertex_buffer);
  pipeline_->SetIndexBuffer(index_buffer, api::DataType::UINT_16);

  stbi_set_flip_vertically_on_load(true);
  int tex_width, tex_height, tex_channels;
  stbi_uc *pixels = stbi_load("../res/textures/image.png", &tex_width,
                              &tex_height, &tex_channels, STBI_rgb_alpha);
  if (pixels == nullptr) {
    throw std::runtime_error("failed to load texture image!");
  }
  auto texture_2_d = rendering_context_->CreateTexture2D(
      static_cast<uint32_t>(tex_width), static_cast<uint32_t>(tex_height),
      api::PixelFormat::RGBA_8_SRGB);
  texture_2_d->Load(pixels);
  stbi_image_free(pixels);
  texture_2_d->SetSampler({api::Filter::LINEAR, api::Filter::LINEAR,
                           api::AddressMode::CLAMP_TO_EDGE,
                           api::AddressMode::CLAMP_TO_EDGE,
                           api::AddressMode::CLAMP_TO_EDGE});
  pipeline_->SetTexture(1, texture_2_d);
}

void test::TestTexture2D::OnRender() {
  ubo_.model = ComputeModelMatrix();
  ubo_.proj = orthographic_projection_;
  ubo_.view = glm::mat4(1.0F);
  pipeline_->UpdateUniformBuffer(0, &ubo_);
  pipeline_->Draw(index_count_, 0);
}

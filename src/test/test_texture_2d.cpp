//
// Created by artyomd on 3/31/20.
//

#include "src/test/test_texture_2d.hpp"

#include <utility>
#include <stb_image.h>
#include <snowhouse/snowhouse.h>

#include "src/shaders/shaders.hpp"

test::TestTexture2D::TestTexture2D(std::shared_ptr<api::RenderingContext> rendering_context)
    : TestModel(std::move(rendering_context)) {

  float positions[] = {
      -0.5F, -0.5F, 0.0F, 0.0F,
      0.5F, -0.5F, 1.0F, 0.0F,
      -0.5F, 0.5F, 0.0F, 1.0F,
      0.5F, 0.5F, 1.0F, 1.0F
  };

  unsigned short indices[] = {
      0, 1, 2,
      2, 1, 3
  };

  api::VertexBufferLayout vertex_buffer_layout;
  size_t stride = sizeof(float) * 4;
  vertex_buffer_layout.Push({0, api::DataType::DATA_TYPE_FLOAT, 2});
  vertex_buffer_layout.Push({1, api::DataType::DATA_TYPE_FLOAT, 2});

  auto vertex_buffer = rendering_context_->CreateVertexBuffer(4 * 4 * sizeof(float), vertex_buffer_layout);
  vertex_buffer->Update(&positions[0]);

  auto index_buffer = rendering_context_->CreateIndexBuffer(6, api::DataType::DATA_TYPE_UINT_16);
  index_buffer->Update(&indices[0]);

  auto vertex_shader = rendering_context_->CreateShader(texture2d_vertex,
                                                        "main",
                                                        api::ShaderType::SHADER_TYPE_VERTEX);
  auto fragment_shader = rendering_context_->CreateShader(texture2d_fragment,
                                                          "main",
                                                          api::ShaderType::SHADER_TYPE_FRAGMENT);
  pipeline_ = rendering_context_->CreateGraphicsPipeline(vertex_buffer,
                                                         index_buffer,
                                                         vertex_shader,
                                                         fragment_shader,
                                                         {
                                                             api::DrawMode::TRIANGLE_LIST,
                                                             api::CullMode::NONE,
                                                             api::FrontFace::CCW,
                                                             false,
                                                             api::CompareOp::LESS
                                                         });

  stbi_set_flip_vertically_on_load(true);
  int tex_width, tex_height, tex_channels;
  stbi_uc *pixels = stbi_load("../res/textures/image.png", &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);
  if (pixels == nullptr) {
    throw std::runtime_error("failed to load texture image!");
  }
  auto texture_2_d =
      rendering_context_->CreateTexture2D(tex_width, tex_height, api::PixelFormat::PIXEL_FORMAT_R8G8B8A8_SRGB);
  texture_2_d->Load(pixels);
  stbi_image_free(pixels);
  texture_2_d->SetSampler({api::Filter::LINEAR,
                           api::Filter::LINEAR,
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
  pipeline_->Render();
}

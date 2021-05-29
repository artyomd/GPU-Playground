//
// Created by artyomd on 5/23/20.
//

#include "src/test/test_cube.hpp"

#include "src/shaders/shaders.hpp"

#include <glm/ext/matrix_transform.hpp>
#include <utility>

test::TestCube::TestCube(std::shared_ptr<api::RenderingContext> rendering_context)
    : TestModel(std::move(rendering_context)) {

  std::vector<float> positions = {
      -0.5, -0.5, 0.5, 1.0, 0.0, 0.0,
      0.5, -0.5, 0.5, 0.0, 1.0, 0.0,
      0.5, 0.5, 0.5, 0.0, 0.0, 1.0,
      -0.5, 0.5, 0.5, 1.0, 1.0, 1.0,
      -0.5, -0.5, -0.5, 1.0, 0.0, 0.0,
      0.5, -0.5, -0.5, 0.0, 1.0, 0.0,
      0.5, 0.5, -0.5, 0.0, 0.0, 1.0,
      -0.5, 0.5, -0.5, 1.0, 1.0, 1.0
  };
  std::vector<unsigned short> indices{
      0, 1, 2,
      2, 3, 0,
      1, 5, 6,
      6, 2, 1,
      7, 6, 5,
      5, 4, 7,
      4, 0, 3,
      3, 7, 4,
      4, 5, 1,
      1, 0, 4,
      3, 2, 6,
      6, 7, 3
  };

  api::VertexBufferLayout vertex_buffer_layout;
  size_t stride = sizeof(float) * 6;
  vertex_buffer_layout.Push({0, api::DataType::DATA_TYPE_FLOAT, 3});
  vertex_buffer_layout.Push({1, api::DataType::DATA_TYPE_FLOAT, 3});
  auto vertex_buffer = rendering_context_->CreateVertexBuffer(positions.size() * sizeof(float), vertex_buffer_layout);
  vertex_buffer->Update(positions.data());

  auto index_buffer = rendering_context_->CreateIndexBuffer(indices.size(), api::DataType::DATA_TYPE_UINT_16);
  index_buffer->Update(indices.data());

  auto vertex_shader = rendering_context_->CreateShader(default_mvp_color_vertex_shader,
                                                        "main",
                                                        api::ShaderType::SHADER_TYPE_VERTEX);
  auto fragment_shader = rendering_context_->CreateShader(default_color_fragment_shader,
                                                          "main",
                                                          api::ShaderType::SHADER_TYPE_FRAGMENT);

  pipeline_ = rendering_context_->CreateGraphicsPipeline(vertex_buffer,
                                                         index_buffer,
                                                         vertex_shader,
                                                         fragment_shader,
                                                         {
                                                             api::DrawMode::TRIANGLE_LIST,
                                                             api::CullMode::BACK,
                                                             api::FrontFace::CCW,
                                                             true,
                                                             api::CompareOp::LESS});
}

void test::TestCube::OnRender() {
  ubo_.model = ComputeModelMatrix();
  ubo_.proj = perspective_projection_;
  ubo_.view = glm::lookAt(glm::vec3(2.0, 2.0, 0.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
  pipeline_->UpdateUniformBuffer(0, &ubo_);
  pipeline_->Render();
}

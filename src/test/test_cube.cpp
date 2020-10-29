//
// Created by artyomd on 5/23/20.
//

#include "src/test/test_cube.hpp"

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
  vertex_buffer_layout.Push<float>(3);
  vertex_buffer_layout.Push<float>(3);
  vertex_buffer_ = rendering_context_->CreateVertexBuffer(positions.size() * sizeof(float), vertex_buffer_layout);
  vertex_buffer_->Update(positions.data());

  index_buffer_ = rendering_context_->CreateIndexBuffer(indices.size(), api::DataType::DATA_TYPE_UINT_16);
  index_buffer_->Update(indices.data());

  vertex_shader_ = rendering_context_->CreateShader("../res/shader/compiled/default_mvp_color_vertex_shader.spv",
                                                    "main",
                                                    api::ShaderType::SHADER_TYPE_VERTEX);
  fragment_shader_ = rendering_context_->CreateShader("../res/shader/compiled/default_color_fragment_shader.spv",
                                                      "main",
                                                      api::ShaderType::SHADER_TYPE_FRAGMENT);

  pipeline_ = rendering_context_->CreateGraphicsPipeline(vertex_buffer_,
                                                         index_buffer_,
                                                         vertex_shader_,
                                                         fragment_shader_,
                                                         {
                                                             api::DrawMode::TRIANGLE_LIST,
                                                             api::CullMode::BACK,
                                                             api::FrontFace::CCW,
                                                             true,
                                                             api::CompareOp::LESS});
  uniform_buffer_ = rendering_context_->CreateUniformBuffer(sizeof(UniformBufferObjectMvp),
                                                            0,
                                                            api::ShaderType::SHADER_TYPE_VERTEX);
  pipeline_->AddUniform(uniform_buffer_);

}

void test::TestCube::OnRender() {
  ubo_->model = ComputeModelMatrix();
  ubo_->proj = perspective_projection_;
  ubo_->view = glm::lookAt(glm::vec3(2.0, 2.0, 0.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
  uniform_buffer_->Update(ubo_.get());
  pipeline_->Render();
}

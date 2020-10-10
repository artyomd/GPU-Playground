//
// Created by artyomd on 5/23/20.
//

#include "src/test/test_cube.hpp"

#include <glm/ext/matrix_transform.hpp>
#include <utility>

test::TestCube::TestCube(std::shared_ptr<api::Renderer> renderer) : TestModel(std::move(renderer)) {

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

  auto context = renderer_->GetContext();

  vertex_buffer_ = context->CreateVertexBuffer(positions.data(), positions.size() * sizeof(float));
  vertex_buffer_layout_->Push<float>(3);
  vertex_buffer_layout_->Push<float>(3);

  vertex_binding_ = context->CreateVertexBinding(vertex_buffer_, vertex_buffer_layout_);
  index_buffer_ = context->CreateIndexBuffer(indices.data(), indices.size(), api::DataType::DATA_TYPE_UINT_16);

  uniform_buffer_ = context->CreateUniformBuffer(sizeof(UniformBufferObjectMvp),
                                                 0,
                                                 api::ShaderType::SHADER_TYPE_VERTEX);
  std::vector<std::shared_ptr<api::Uniform>> uniforms;
  uniforms.push_back(uniform_buffer_);
  pipeline_layout_ = context->CreateRenderingPipelineLayout(uniforms);
  vertex_shader_ = context->CreateShader("../res/shader/compiled/default_mvp_color_vertex_shader.spv",
                                         "../res/shader/default_mvp_color_vertex_shader.glsl",
                                         "main",
                                         api::ShaderType::SHADER_TYPE_VERTEX);
  fragment_shader_ = context->CreateShader("../res/shader/compiled/default_color_fragment_shader.spv",
                                           "../res/shader/default_color_fragment_shader.glsl",
                                           "main",
                                           api::ShaderType::SHADER_TYPE_FRAGMENT);
  pipeline_ = context->CreateGraphicsPipeline(vertex_binding_, index_buffer_,
                                              vertex_shader_, fragment_shader_, pipeline_layout_,
                                              {api::CullMode::BACK,
                                               api::FrontFace::CCW,
                                               true,
                                               api::DepthFunction::LESS});
}

void test::TestCube::OnClear() {
  renderer_->SetClearColor({0.0f, 0.0f, 0.0f, 1.0f});
}

void test::TestCube::OnRender() {
  ubo_->model = ComputeModelMatrix();
  ubo_->proj = renderer_->GetContext()->GetPerspectiveProjection();
  ubo_->view = glm::lookAt(glm::vec3(2.0, 2.0, 0.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
  uniform_buffer_->Update(ubo_.get());
  api::Renderer::Render(pipeline_);
}
void test::TestCube::OnViewportChange() {
  pipeline_->ViewportChanged();
}

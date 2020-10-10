//
// Created by artyomd on 3/23/20.
//
#include "src/test/test_sphere.hpp"

#include <utility>

#include "src/geometry/stacked_sphere.hpp"
#include "src/geometry/spiral_sphere.hpp"

test::TestSphere::TestSphere(std::shared_ptr<api::Renderer> renderer) : TestModel(std::move(renderer)) {
  auto context = renderer_->GetContext();

  uniform_buffer_ = context->CreateUniformBuffer(sizeof(UniformBufferObjectMvp),
                                                 0,
                                                 api::ShaderType::SHADER_TYPE_VERTEX);
  std::vector<std::shared_ptr<api::Uniform>> uniforms;
  uniforms.push_back(uniform_buffer_);
  pipeline_layout_ = context->CreateRenderingPipelineLayout(uniforms);

  sphere_ = std::make_shared<geometry::SpiralSphere>(context, 1.0f);
  vertex_shader_ = context->CreateShader("../res/shader/compiled/default_mvp_color_vertex_shader.spv",
                                         "../res/shader/default_mvp_color_vertex_shader.glsl",
                                         "main",
                                         api::ShaderType::SHADER_TYPE_VERTEX);
  fragment_shader_ = context->CreateShader("../res/shader/compiled/default_color_fragment_shader.spv",
                                           "../res/shader/default_color_fragment_shader.glsl",
                                           "main",
                                           api::ShaderType::SHADER_TYPE_FRAGMENT);
  pipeline_ = context->CreateGraphicsPipeline(sphere_->GetVertexBinding(), sphere_->GetIndexBuffer(),
                                              vertex_shader_, fragment_shader_, pipeline_layout_,
                                              {api::CullMode::NONE,
                                               api::FrontFace::CCW,
                                               true,
                                               api::DepthFunction::LESS});
}
void test::TestSphere::OnClear() {
  renderer_->SetClearColor({0.0f, 0.0f, 0.0f, 1.0f});
}
void test::TestSphere::OnRender() {
  ubo_->model = ComputeModelMatrix();
  ubo_->proj = renderer_->GetContext()->GetOrthographicProjection();
  ubo_->view = glm::mat4(1.0f);
  uniform_buffer_->Update(ubo_.get());
  api::Renderer::Render(pipeline_);
}
void test::TestSphere::OnViewportChange() {
  pipeline_->ViewportChanged();
}

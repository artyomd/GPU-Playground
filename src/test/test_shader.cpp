//
// Created by artyomd on 3/21/20.
//

#include "src/test/test_shader.hpp"

#include <utility>

#include "src/geometry/point.hpp"
#include "src/geometry/quad.hpp"

test::TestShader::TestShader(std::shared_ptr<api::RenderingContext> rendering_context,
                             std::string fragment_shader_data)
    : Test(std::move(rendering_context)) {
  geometry::Point point_0 = {-1.0F, 1.0F, 0.0F};
  geometry::Point point_1 = {1.0F, 1.0F, 0.0F};
  geometry::Point point_2 = {1.0F, -1.0F, 0.0F};
  geometry::Point point_3 = {-1.0F, -1.0F, 0.0F};

  auto quad = std::make_shared<geometry::Quad>(rendering_context_, point_0, point_1, point_2, point_3);

  auto vertex_shader = rendering_context_->CreateShader(default_empty_vertex_shader,
                                                        "main",
                                                        api::ShaderType::SHADER_TYPE_VERTEX);

  auto fragment_shader = rendering_context_->CreateShader(std::move(fragment_shader_data),
                                                          "main",
                                                          api::ShaderType::SHADER_TYPE_FRAGMENT);

  pipeline_ = rendering_context_->CreateGraphicsPipeline(quad->GetVertexBuffer(),
                                                         quad->GetIndexBuffer(),
                                                         vertex_shader,
                                                         fragment_shader,
                                                         {
                                                             api::DrawMode::TRIANGLE_STRIP,
                                                             api::CullMode::NONE,
                                                             api::FrontFace::CW,
                                                             false,
                                                             api::CompareOp::LESS
                                                         });
  pipeline_->SetViewPort(size_[0], size_[1]);
}

void test::TestShader::OnRender() {
  pipeline_->UpdateUniformBuffer(0, &ubo_);
  pipeline_->Render();
}

void test::TestShader::OnUpdate(float delta_time) {
  ubo_.time += delta_time;
}

void test::TestShader::OnImGuiRender() {
//  if (ImGui::Button("Reload")) {
//    shader->recompile();
//  }
}

void test::TestShader::OnViewportChange(size_t width, size_t height) {
  Test::OnViewportChange(width, height);
  ubo_.screen_height = static_cast<float>(height);
  ubo_.screen_width = static_cast<float>(width);
  pipeline_->SetViewPort(size_[0], size_[1]);
}


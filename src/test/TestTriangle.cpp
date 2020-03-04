//
// Created by Artyom Dangizyan on 2018-11-29.
//

#include <iostream>
#include "geometry/Point.hpp"
#include "geometry/Triangle.h"
#include "TestTriangle.h"

namespace test {

TestTriangle::TestTriangle(api::Renderer *renderer) : Test(renderer) {

  geometry::Point point_0 = {-0.5f, -0.5f, 0.0f, 1, 1, 0, 0};
  geometry::Point point_1 = {0.5f, -0.5f, 0.0f, 1, 0, 1, 0};
  geometry::Point point_2 = {0.0f, 0.5f, 0.0f, 1, 0, 0, 1};

  auto *context = renderer->GetRenderingContext();

  triangle_ = new geometry::Triangle(context, point_0, point_1, point_2);
  vertex_shader_ = context->CreateShader("../res/shader/compiled/testTriangleV.spv",
                                         "../res/shader/testTriangleV.glsl",
                                         "main",
                                         api::SHADER_TYPE_VERTEX);
  fragment_shader_ = context->CreateShader("../res/shader/compiled/testTriangleF.spv",
                                           "../res/shader/testTriangleF.glsl",
                                           "main",
                                           api::SHADER_TYPE_FRAGMENT);
  pipeline_ = context->CreateGraphicsPipeline(triangle_->GetVertexBinding(), triangle_->GetIndexBuffer(),
                                              vertex_shader_, fragment_shader_);
}

void TestTriangle::OnClear() {
  renderer_->SetClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void TestTriangle::OnRender() {
  pipeline_->Render();
}

TestTriangle::~TestTriangle() {
  auto context = renderer_->GetRenderingContext();
  context->FreeGraphicsPipeline(pipeline_);
  context->DeleteShader(fragment_shader_);
  context->DeleteShader(vertex_shader_);
  delete triangle_;
}
}
//
// Created by artyomd on 3/30/20.
//

#include "src/api/opengl/opengl_rendering_pipeline_layout.hpp"

#include "src/api/opengl/opengl_uniform.hpp"

api::opengl::OpenGlRenderingPipelineLayout::OpenGlRenderingPipelineLayout(
    std::vector<std::shared_ptr<Uniform>> bindings)
    : RenderingPipelineLayout(std::move(bindings)) {}

void api::opengl::OpenGlRenderingPipelineLayout::Bind() const {
  for (const auto &value: bindings_) {
    std::dynamic_pointer_cast<OpenGlUniform>(value)->Bind();
  }
}
void api::opengl::OpenGlRenderingPipelineLayout::UnBind() const {
  for (const auto &value: bindings_) {
    std::dynamic_pointer_cast<OpenGlUniform>(value)->Unbind();
  }
}

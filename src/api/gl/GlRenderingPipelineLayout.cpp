//
// Created by artyomd on 3/30/20.
//

#include "src/api/gl/GlRenderingPipelineLayout.h"

#include "src/api/gl/GlUniform.h"

api::GlRenderingPipelineLayout::GlRenderingPipelineLayout(std::vector<std::shared_ptr<Uniform>> bindings)
    : RenderingPipelineLayout(std::move(bindings)) {}

void api::GlRenderingPipelineLayout::Bind() const {
  for (const auto &value: bindings_) {
    std::dynamic_pointer_cast<GlUniform>(value)->Bind();
  }
}
void api::GlRenderingPipelineLayout::UnBind() const {
  for (const auto &value: bindings_) {
    std::dynamic_pointer_cast<GlUniform>(value)->Unbind();
  }
}

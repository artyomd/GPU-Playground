//
// Created by artyomd on 3/30/20.
//

#include "src/api/gl/GlRenderingPipelineLayout.h"

#include "src/api/gl/GlUniform.h"

api::GlRenderingPipelineLayout::GlRenderingPipelineLayout(const std::vector<Uniform *> &bindings)
    : RenderingPipelineLayout(bindings) {}

void api::GlRenderingPipelineLayout::Bind() const {
  for (auto value: bindings_) {
    dynamic_cast<GlUniform *>(value)->Bind();
  }
}
void api::GlRenderingPipelineLayout::UnBind() const {
  for (auto value: bindings_) {
    dynamic_cast<GlUniform *>(value)->Unbind();
  }
}

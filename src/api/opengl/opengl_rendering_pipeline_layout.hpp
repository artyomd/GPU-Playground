//
// Created by artyomd on 3/30/20.
//

#pragma once

#include "src/api/rendering_pipeline_layout.hpp"

namespace api::opengl {
class OpenGlRenderingPipelineLayout : public RenderingPipelineLayout {
 public:
  explicit OpenGlRenderingPipelineLayout(std::vector<std::shared_ptr<Uniform>> bindings);
  void Bind() const override;
  void UnBind() const;
};
}
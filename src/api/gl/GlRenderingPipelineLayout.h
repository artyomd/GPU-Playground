//
// Created by artyomd on 3/30/20.
//

#pragma once

#include "src/api/RenderingPipelineLayout.h"

namespace api {
class GlRenderingPipelineLayout : public RenderingPipelineLayout {
 public:
  explicit GlRenderingPipelineLayout(std::vector<std::shared_ptr<Uniform>> bindings);
  void Bind() const override;
  void UnBind() const;
};
}
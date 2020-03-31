//
// Created by artyomd on 3/30/20.
//

#pragma once

#include <api/RenderingPipelineLayout.h>
namespace api {
class GlRenderingPipelineLayout : public RenderingPipelineLayout {
 public:
  GlRenderingPipelineLayout(const std::vector<Uniform *> &bindings);
  void Bind() const override;
  void UnBind() const;
};
}
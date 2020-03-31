//
// Created by artyomd on 3/31/20.
//

#pragma once

#include <api/Texture2D.h>
#include "GlUniform.h"

namespace api {
class GlTexture2D : public Texture2D, public GlUniform {
 public:
  GlTexture2D(const std::string &image_path, int binding_point, ShaderType shader_stage);
  ~GlTexture2D() override;
  void Bind() const override;
  void Unbind() const override;
};
}
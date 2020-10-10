//
// Created by artyomd on 3/31/20.
//

#pragma once

#include "src/api/texture_2d.hpp"
#include "src/api/opengl/opengl_uniform.hpp"

namespace api::opengl {
class OpenglTexture2D : public Texture2D, public OpenGlUniform {
 public:
  OpenglTexture2D(const std::string &image_path, int binding_point, ShaderType shader_stage);
  ~OpenglTexture2D() override;
  void Bind() const override;
  void Unbind() const override;
};
}
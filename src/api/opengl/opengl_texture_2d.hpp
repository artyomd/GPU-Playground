//
// Created by artyomd on 3/31/20.
//

#pragma once

#include "src/api/texture_2d.hpp"
#include "src/api/opengl/opengl_uniform.hpp"

namespace api::opengl {
class OpenglTexture2D : public Texture2D, public OpenGlUniform {
 public:
  OpenglTexture2D(int binding_point, ShaderType shader_stage);
  void Load(const std::string &path) override;
  void Load(size_t width, size_t height, void *data) override;
  void SetSampler(api::Sampler sampler) override;
  void Bind() const override;
  void Unbind() const override;
  ~OpenglTexture2D() override;
};
}

//
// Created by artyomd on 3/31/20.
//

#pragma once

#include <GL/glew.h>
#include <string>

#include "src/api/texture_2d.hpp"

namespace api::opengl {
class OpenglTexture2D : public Texture2D {
 public:
  OpenglTexture2D(uint32_t width, uint32_t height, PixelFormat pixel_format);
  void SetSampler(api::Sampler sampler) override;
  void Load(const void *data) override;
  void Bind(GLuint binding_point) const;
  static void Unbind(GLuint binding_point);
  ~OpenglTexture2D() override;
 private:
  GLuint texture_id_ = 0;
};
}

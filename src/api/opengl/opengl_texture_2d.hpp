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
  OpenglTexture2D(size_t width, size_t height, PixelFormat pixel_format);
  void SetSampler(api::Sampler sampler) override;
  void Load(const void *data) override;
  void Bind(GLuint binding_point) const;
  void Unbind(GLuint binding_point) const;
  ~OpenglTexture2D();
 private:
  GLuint texture_id_ = 0;
};
}

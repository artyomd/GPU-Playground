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
  OpenglTexture2D();
  void Load(const std::string &path) override;
  void Load(size_t width, size_t height, const void *data) override;
  void Bind(GLuint binding_point) const;
  void Unbind(GLuint binding_point) const;
  void SetSampler(api::Sampler sampler) override;
  ~OpenglTexture2D();
 private:
  GLuint texture_id_ = 0;
};
}

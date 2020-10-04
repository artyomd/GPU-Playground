//
// Created by artyomd on 3/30/20.
//

#pragma once

#include "src/api/Uniform.h"
#include "src/api/gl/GlUtils.h"

namespace api {
class GlUniform {
 protected:
  GLuint render_id_ = -1;
 public:
  virtual void Bind() const = 0;
  virtual void Unbind() const = 0;
};
}
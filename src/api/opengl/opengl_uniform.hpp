//
// Created by artyomd on 3/30/20.
//

#pragma once

#include "src/api/uniform.hpp"
#include "src/api/opengl/opengl_utils.hpp"

namespace api::opengl {
class OpenGlUniform {
 protected:
  GLuint render_id_ = -1;
 public:
  virtual void Bind() const = 0;
  virtual void Unbind() const = 0;
};
}

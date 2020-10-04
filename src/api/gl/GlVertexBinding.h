//
// Created by Artyom Dangizyan on 10/21/18.
//
#pragma once

#include <GL/glew.h>

#include "src/api/gl/GlUtils.h"
#include "src/api/VertexBinding.h"
#include "src/api/VertexBuffer.h"
#include "src/api/VertexBufferLayout.h"

namespace api {
class GlVertexBinding : public VertexBinding {
 private:
  GLuint renderer_id_ = -1;
 public:
  GlVertexBinding(const VertexBuffer *vb, const VertexBufferLayout *layout);

  void Bind() const override;
  void Unbind() const override;

  ~GlVertexBinding() override;
};
}
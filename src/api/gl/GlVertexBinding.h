//
// Created by Artyom Dangizyan on 10/21/18.
//
#pragma once

#include "VertexBuffer.h"
#include "VertexBinding.h"
#include "VertexBufferLayout.h"
#include <GL/glew.h>
#include "GlUtils.h"

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
//
// Created by Artyom Dangizyan on 10/8/18.
//

#pragma once

#include "IndexBuffer.h"
#include <GL/glew.h>

namespace api {
class GlIndexBuffer : public IndexBuffer {
 private:
  GLuint renderer_id_ = -1;
 public:
  GlIndexBuffer(const void *data, unsigned int item_count, enum DataType type);

  ~GlIndexBuffer() override;

  void Bind() const override;

  void Unbind() const override;
};
}

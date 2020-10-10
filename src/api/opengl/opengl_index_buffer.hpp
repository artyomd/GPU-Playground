//
// Created by Artyom Dangizyan on 10/8/18.
//

#pragma once

#include <GL/glew.h>

#include "src/api/index_buffer.hpp"

namespace api::opengl {
class OpenGlIndexBuffer : public IndexBuffer {
 private:
  GLuint renderer_id_ = -1;
 public:
  OpenGlIndexBuffer(const void *data, unsigned int item_count, enum DataType type);

  ~OpenGlIndexBuffer() override;

  void Bind() const override;

  void Unbind() const override;
};
}

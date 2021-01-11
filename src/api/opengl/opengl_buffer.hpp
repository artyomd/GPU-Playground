//
// Created by artyomd on 1/11/21.
//

#pragma once

#include <GL/glew.h>

#include "src/api/buffer.hpp"

namespace api::opengl {
class OpenGlBuffer : virtual public Buffer {
 public:
  OpenGlBuffer() = delete;
  explicit OpenGlBuffer(const size_t &length);
  [[nodiscard]] GLuint GetBufferId() const;
  void Update(const void *data) override;
  void CopyFrom(std::shared_ptr<api::Buffer> src_buffer, size_t size, size_t src_offset, size_t dst_offset) override;
  ~OpenGlBuffer() override;
 protected:
  GLuint buffer_id_ = 0;
};
}

//
// Created by Artyom Dangizyan on 10/8/18.
//

#pragma once

#include <GL/glew.h>

#include "src/api/index_buffer.hpp"

namespace api::opengl {
class OpenGlIndexBuffer : public IndexBuffer {
 private:
  GLuint buffer_id_ = 0;
  GLint index_type_ = 0;

 public:
  OpenGlIndexBuffer(unsigned int item_count, enum DataType type);

  void Update(void *data) override;
  [[nodiscard]] GLuint GetBufferId() const;
  [[nodiscard]] GLint GetIndexType() const;

  ~OpenGlIndexBuffer() override;
};
}

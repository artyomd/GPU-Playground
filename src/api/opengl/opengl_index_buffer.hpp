//
// Created by Artyom Dangizyan on 10/8/18.
//

#pragma once

#include "src/api/index_buffer.hpp"
#include "src/api/opengl/opengl_buffer.hpp"

namespace api::opengl {
class OpenGlIndexBuffer :
    public OpenGlBuffer,
    public IndexBuffer {
 private:
  GLenum index_type_ = 0;

 public:
  OpenGlIndexBuffer(uint32_t item_count, enum DataType type);

  [[nodiscard]] GLenum GetIndexType() const;

  ~OpenGlIndexBuffer() override = default;
};
}

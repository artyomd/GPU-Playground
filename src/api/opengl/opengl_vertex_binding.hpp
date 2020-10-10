//
// Created by Artyom Dangizyan on 10/21/18.
//
#pragma once

#include <GL/glew.h>

#include "src/api/opengl/opengl_utils.hpp"
#include "src/api/vertex_binding.hpp"
#include "src/api/vertex_buffer.hpp"
#include "src/api/vertex_buffer_layout.hpp"

namespace api::opengl {
class OpenGlVertexBinding : public VertexBinding {
 private:
  GLuint renderer_id_ = -1;
 public:
  OpenGlVertexBinding(std::shared_ptr<VertexBuffer> vb, std::shared_ptr<VertexBufferLayout> layout);

  void Bind() const override;
  void Unbind() const override;

  ~OpenGlVertexBinding() override;
};
}
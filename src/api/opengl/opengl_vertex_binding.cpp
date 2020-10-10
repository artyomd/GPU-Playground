//
// Created by Artyom Dangizyan on 10/21/18.
//

#include "src/api/opengl/opengl_vertex_binding.hpp"

#include <utility>

#include "src/api/data_type.hpp"
#include "src/api/opengl/opengl_utils.hpp"
#include "src/api/opengl/opengl_vertex_buffer.hpp"

api::opengl::OpenGlVertexBinding::OpenGlVertexBinding(std::shared_ptr<VertexBuffer> vb,
                                                      std::shared_ptr<VertexBufferLayout> layout)
    : VertexBinding(std::move(vb), std::move(layout)) {
  GL_CALL(glGenVertexArrays(1, &renderer_id_));
  GL_CALL(glBindVertexArray(renderer_id_));
  buffer_->Bind();
  const auto &elements = buffer_description_->GetElements();
  unsigned int offset = 0;
  for (unsigned int i = 0; i < elements.size(); i++) {
    const auto &element = elements[i];
    GL_CALL(glEnableVertexAttribArray(i));
    GL_CALL(glVertexAttribPointer(i, element.count, GetGlType(element.type), GL_FALSE, buffer_description_->GetStride(),
                                  reinterpret_cast<void *> (offset)));
    offset += element.count * GetDataTypeSizeInBytes(element.type);
  }
  buffer_->Unbind();
  GL_CALL(glBindVertexArray(0));
}

void api::opengl::OpenGlVertexBinding::Bind() const {
  GL_CALL(glBindVertexArray(renderer_id_));
}

void api::opengl::OpenGlVertexBinding::Unbind() const {
  GL_CALL(glBindVertexArray(0));
}

api::opengl::OpenGlVertexBinding::~OpenGlVertexBinding() {
  GL_CALL(glDeleteVertexArrays(1, &renderer_id_));
}

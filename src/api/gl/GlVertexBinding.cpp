//
// Created by Artyom Dangizyan on 10/21/18.
//

#include "src/api/gl/GlVertexBinding.h"

#include "src/api/DataType.h"
#include "src/api/gl/GlUtils.h"
#include "src/api/gl/GlVertexBuffer.h"

namespace api {
GlVertexBinding::GlVertexBinding(const VertexBuffer *vb, const VertexBufferLayout *layout) :
    VertexBinding(vb, layout) {
  GL_CALL(glGenVertexArrays(1, &renderer_id_));
  GL_CALL(glBindVertexArray(renderer_id_));
  vb->Bind();
  const auto &elements = layout->GetElements();
  unsigned int offset = 0;
  for (unsigned int i = 0; i < elements.size(); i++) {
    const auto &element = elements[i];
    GL_CALL(glEnableVertexAttribArray(i));
    GL_CALL(glVertexAttribPointer(i, element.count_, GetGlType(element.type_), GL_FALSE, layout->GetStride(),
                                  reinterpret_cast<void *> (offset)));
    offset += element.count_ * GetDataTypeSizeInBytes(element.type_);
  }
  vb->Unbind();
  GL_CALL(glBindVertexArray(0));
}

void GlVertexBinding::Bind() const {
  GL_CALL(glBindVertexArray(renderer_id_));
}

void GlVertexBinding::Unbind() const {
  GL_CALL(glBindVertexArray(0));
}

GlVertexBinding::~GlVertexBinding() {
  GL_CALL(glDeleteVertexArrays(1, &renderer_id_));
}
}
#include "src/api/opengl/opengl_buffer.hpp"

#include "src/api/opengl/opengl_utils.hpp"
#include "src/utils/check.hpp"

api::opengl::OpenGlBuffer::OpenGlBuffer(const size_t &length) : Buffer(length) {
  GL_CALL(glCreateBuffers(1, &buffer_id_));
  CHECK(buffer_id_ != 0, "uable to create a buffer object")
  GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, buffer_id_));
  GL_CALL(glBufferData(GL_ARRAY_BUFFER, size_in_bytes_, nullptr, GL_STATIC_DRAW));
  GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

GLuint api::opengl::OpenGlBuffer::GetBufferId() const { return buffer_id_; }

void api::opengl::OpenGlBuffer::Update(const void *data) {
  GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, buffer_id_));
  GL_CALL(glBufferData(GL_ARRAY_BUFFER, size_in_bytes_, data, GL_STATIC_DRAW));
  GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

void api::opengl::OpenGlBuffer::CopyFrom(std::shared_ptr<api::Buffer> src_buffer,
                                         size_t size,
                                         size_t src_offset,
                                         size_t dst_offset) {
  auto gl_buffer = std::dynamic_pointer_cast<OpenGlBuffer>(src_buffer);
  GL_CALL(glBindBuffer(GL_COPY_READ_BUFFER, gl_buffer->GetBufferId()));
  GL_CALL(glBindBuffer(GL_COPY_WRITE_BUFFER, buffer_id_));
  GL_CALL(glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, src_offset, dst_offset, size));
  GL_CALL(glBindBuffer(GL_COPY_READ_BUFFER, 0));
  GL_CALL(glBindBuffer(GL_COPY_WRITE_BUFFER, 0));
}

api::opengl::OpenGlBuffer::~OpenGlBuffer() {
  GL_CALL(glDeleteBuffers(1, &buffer_id_));
}

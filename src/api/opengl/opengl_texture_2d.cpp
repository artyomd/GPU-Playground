//
// Created by artyomd on 3/31/20.
//

#include "src/api/opengl/opengl_texture_2d.hpp"

#include "src/api/opengl/opengl_utils.hpp"

api::opengl::OpenglTexture2D::OpenglTexture2D(uint32_t width, uint32_t height,
                                              PixelFormat pixel_format)
    : Texture2D(width, height, pixel_format) {}

void api::opengl::OpenglTexture2D::Load(const void *data) {
  if (texture_id_ != 0) {
    GL_CALL(glDeleteTextures(1, &texture_id_));
  }
  GL_CALL(glGenTextures(1, &texture_id_));
  GL_CALL(glBindTexture(GL_TEXTURE_2D, texture_id_));

  GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
  GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
  GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
  GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

  GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, static_cast<GLsizei>(width_),
                       static_cast<GLsizei>(height_), 0, GL_RGBA,
                       GL_UNSIGNED_BYTE, data));
  GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
}

void api::opengl::OpenglTexture2D::Bind(GLuint binding_point) const {
  GL_CALL(glActiveTexture(GL_TEXTURE0 + binding_point));
  GL_CALL(glBindTexture(GL_TEXTURE_2D, texture_id_));
}

void api::opengl::OpenglTexture2D::Unbind(GLuint binding_point) {
  glActiveTexture(GL_TEXTURE0 + binding_point);
  GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
}

void api::opengl::OpenglTexture2D::SetSampler(api::Sampler sampler) {
  GL_CALL(glBindTexture(GL_TEXTURE_2D, texture_id_));
  GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                          GetGlFilter(sampler.min_filter)));
  GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                          GetGlFilter(sampler.mag_filter)));
  GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                          GetGlAddressMode(sampler.address_mode_u)));
  GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                          GetGlAddressMode(sampler.address_mode_v)));
  GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R,
                          GetGlAddressMode(sampler.address_mode_w)));
  GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
}

api::opengl::OpenglTexture2D::~OpenglTexture2D() {
  GL_CALL(glDeleteTextures(1, &texture_id_));
}

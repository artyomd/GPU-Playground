//
// Created by artyomd on 3/31/20.
//

#include "src/api/opengl/opengl_texture_2d.hpp"

#include <stb_image.h>
#include <snowhouse/snowhouse.h>

#include "src/api/opengl/opengl_utils.hpp"

api::opengl::OpenglTexture2D::OpenglTexture2D()
    : Texture2D() {
}

void api::opengl::OpenglTexture2D::Load(const std::string &path) {
  if (texture_id_ != 0) {
    GL_CALL(glDeleteTextures(1, &texture_id_));
  }
  stbi_set_flip_vertically_on_load(true);
  GLsizei tex_width, tex_height, tex_channels;
  stbi_uc *pixels = stbi_load(path.c_str(), &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);
  if (pixels != nullptr) {
    throw std::runtime_error("failed to load texture image!");
  }
  GL_CALL(glGenTextures(1, &texture_id_));
  GL_CALL(glBindTexture(GL_TEXTURE_2D, texture_id_));

  GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
  GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
  GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
  GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

  GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels));
  GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));

  stbi_image_free(pixels);
}

void api::opengl::OpenglTexture2D::Load(size_t width, size_t height, const void *data) {
  if (texture_id_ != 0) {
    GL_CALL(glDeleteTextures(1, &texture_id_));
  }
  GL_CALL(glGenTextures(1, &texture_id_));
  GL_CALL(glBindTexture(GL_TEXTURE_2D, texture_id_));

  GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
  GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
  GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
  GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

  GL_CALL(glTexImage2D(GL_TEXTURE_2D,
                       0,
                       GL_RGBA,
                       static_cast<GLsizei>(width),
                       static_cast<GLsizei>(height),
                       0,
                       GL_RGBA,
                       GL_UNSIGNED_BYTE,
                       data));
  GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
}

void api::opengl::OpenglTexture2D::Bind(GLuint binding_point) const {
  GL_CALL(glActiveTexture(GL_TEXTURE0 + binding_point));
  GL_CALL(glBindTexture(GL_TEXTURE_2D, texture_id_));
}

void api::opengl::OpenglTexture2D::Unbind(GLuint binding_point) const {
  glActiveTexture(GL_TEXTURE0 + binding_point);
  GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
}

void api::opengl::OpenglTexture2D::SetSampler(api::Sampler sampler) {
  GL_CALL(glBindTexture(GL_TEXTURE_2D, texture_id_));
  GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GetGlFilter(sampler.min_filter)));
  GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GetGlFilter(sampler.mag_filter)));
  GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GetGlAddressMode(sampler.address_mode_u)));
  GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GetGlAddressMode(sampler.address_mode_v)));
  GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GetGlAddressMode(sampler.address_mode_w)));
  GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
}

api::opengl::OpenglTexture2D::~OpenglTexture2D() {
  GL_CALL(glDeleteTextures(1, &texture_id_));
}

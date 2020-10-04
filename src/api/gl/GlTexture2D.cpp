//
// Created by artyomd on 3/31/20.
//

#include "src/api/gl/GlTexture2D.h"

#include <stb_image/stb_image.h>
#include <stdexcept>

#include "src/api/gl/GlUtils.h"

api::GlTexture2D::GlTexture2D(const std::string &image_path, int binding_point, api::ShaderType shader_stage)
    : Texture2D(image_path, binding_point, shader_stage) {

  int tex_width, tex_height, tex_channels;
  stbi_uc *pixels = stbi_load(image_path_.c_str(), &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);
  if (!pixels) {
    throw std::runtime_error("failed to load texture image!");
  }
  GL_CALL(glGenTextures(1, &render_id_));
  GL_CALL(glBindTexture(GL_TEXTURE_2D, render_id_));

  GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
  GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
  GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
  GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

  GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, tex_width, tex_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels));
  GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));

  stbi_image_free(pixels);
}
void api::GlTexture2D::Bind() const {
  GL_CALL(glActiveTexture(GL_TEXTURE0 + binding_point_));
  GL_CALL(glBindTexture(GL_TEXTURE_2D, render_id_));
}
void api::GlTexture2D::Unbind() const {
  GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
}

api::GlTexture2D::~GlTexture2D() {
  GL_CALL(glDeleteTextures(1, &render_id_));
}
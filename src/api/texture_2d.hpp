//
// Created by artyomd on 3/31/20.
//

#pragma once

#include <string>
#include <utility>

#include "src/api/sampler.hpp"

namespace api {
class Texture2D {
 protected:
  size_t width_;
  size_t height_;
  PixelFormat pixel_format_;
  Texture2D(size_t width, size_t height, PixelFormat pixel_format)
      : width_(width), height_(height), pixel_format_(pixel_format) {};
 public:
  virtual void SetSampler(api::Sampler sampler) = 0;

  virtual void Load(const void *data) = 0;

  [[nodiscard]] size_t GetWidth() const {
    return width_;
  }
  [[nodiscard]] size_t GetHeight() const {
    return height_;
  }
  [[nodiscard]] PixelFormat GetPixelFormat() const {
    return pixel_format_;
  }
  [[nodiscard]] size_t GetTextureSizeInBytes() const {
    return GetPixelFormatSizeInBytes(pixel_format_) * width_ * height_;
  }
};
}

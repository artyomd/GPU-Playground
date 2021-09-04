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
  uint32_t width_;
  uint32_t height_;
  PixelFormat pixel_format_;
  Texture2D(uint32_t width, uint32_t height, PixelFormat pixel_format)
      : width_(width), height_(height), pixel_format_(pixel_format) {};
 public:
  virtual void SetSampler(api::Sampler sampler) = 0;

  virtual void Load(const void *data) = 0;

  [[nodiscard]] uint32_t GetWidth() const {
    return width_;
  }

  [[nodiscard]] uint32_t GetHeight() const {
    return height_;
  }

  [[nodiscard]] PixelFormat GetPixelFormat() const {
    return pixel_format_;
  }

  [[nodiscard]] size_t GetTextureSizeInBytes() const {
    return GetPixelFormatSizeInBytes(pixel_format_) * width_ * height_;
  }
  virtual ~Texture2D() = default;
};
}

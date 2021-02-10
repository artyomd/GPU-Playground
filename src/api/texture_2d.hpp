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
  Texture2D() = default;
 public:
  virtual void SetSampler(api::Sampler sampler) = 0;

  virtual void Load(const std::string &path) = 0;

  virtual void Load(size_t width, size_t height, const void *data) = 0;
};
}

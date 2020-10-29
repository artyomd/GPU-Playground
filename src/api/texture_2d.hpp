//
// Created by artyomd on 3/31/20.
//

#pragma once

#include <utility>

#include "src/api/sampler.hpp"
#include "src/api/uniform.hpp"

namespace api {
class Texture2D : public Uniform {
 protected:
  std::string image_path_;
  Texture2D(std::string image_path, int binding_point, ShaderType shader_stage) :
      Uniform(binding_point, shader_stage), image_path_(std::move(image_path)) {};

 public:
  virtual void SetSampler(api::Sampler sampler) = 0;

  [[nodiscard]] const std::string &GetImagePath() const {
    return image_path_;
  }
};
}

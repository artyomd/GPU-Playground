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
  Texture2D(int binding_point, ShaderType shader_stage) :
      Uniform(binding_point, shader_stage) {};

 public:
  virtual void SetSampler(api::Sampler sampler) = 0;

  virtual void Load(const std::string &path) = 0;

  virtual void Load(size_t width, size_t height, void *data) = 0;
};
}

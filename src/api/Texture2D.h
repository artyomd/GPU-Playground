//
// Created by artyomd on 3/31/20.
//

#pragma once

#include <utility>

#include "Uniform.h"
namespace api {
class Texture2D : public Uniform {
 protected:
  std::string image_path_;
 public:
  Texture2D(std::string image_path, int binding_point, ShaderType shader_stage) :
      Uniform(binding_point, shader_stage), image_path_(std::move(image_path)) {}
};
}
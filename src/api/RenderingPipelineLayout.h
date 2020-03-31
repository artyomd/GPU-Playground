//
// Created by artyomd on 3/25/20.
//
#pragma once

#include <vector>
#include <utility>
#include "Uniform.h"

namespace api {
class RenderingPipelineLayout {
 protected:
  std::vector<Uniform *> bindings_;
 public:
  explicit RenderingPipelineLayout(std::vector<Uniform *> bindings) : bindings_(std::move(bindings)) {}
  virtual void Bind() const = 0;
  virtual ~RenderingPipelineLayout() = default;
};
}
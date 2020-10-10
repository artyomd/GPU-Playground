//
// Created by artyomd on 3/25/20.
//
#pragma once

#include <utility>
#include <vector>
#include <utility>
#include <memory>

#include "src/api/uniform.hpp"

namespace api {
class RenderingPipelineLayout {
 protected:
  std::vector<std::shared_ptr<Uniform>> bindings_;
 public:
  explicit RenderingPipelineLayout(std::vector<std::shared_ptr<Uniform>> bindings) : bindings_(std::move(bindings)) {}
  virtual void Bind() const = 0;
  virtual ~RenderingPipelineLayout() = default;
};
}
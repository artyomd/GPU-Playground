//
// Created by artyomd on 1/1/20.
//
#pragma once

#include <utility>
#include <memory>

#include "src/api/index_buffer.hpp"
#include "src/api/redering_pipeline_config.hpp"
#include "src/api/shader.hpp"
#include "src/api/vertex_buffer.hpp"
#include "src/api/uniform.hpp"

namespace api {
class RenderingPipeline {
 protected:
  explicit RenderingPipeline() = default;
 public:

  virtual void AddUniform(std::shared_ptr<api::Uniform> uniform) = 0;

  virtual void Render() = 0;

  virtual void ViewportChanged(size_t width, size_t height) = 0;

  virtual ~RenderingPipeline() = default;
};
}

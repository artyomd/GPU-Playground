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
#include "src/api/uniform_buffer.hpp"
#include "src/api/texture_2d.hpp"

namespace api {
class RenderingPipeline {
 protected:
  explicit RenderingPipeline() = default;
 public:
  virtual void UpdateUniformBuffer(unsigned int binding_point, void *data) = 0;

  virtual void SetTexture(unsigned int binding_point, std::shared_ptr<api::Texture2D> texture) = 0;

  virtual void SetViewPort(size_t width, size_t height) = 0;

  virtual void Render() = 0;

  virtual ~RenderingPipeline() = default;
};
}

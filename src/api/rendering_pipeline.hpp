//
// Created by artyomd on 1/1/20.
//
#pragma once

#include <utility>
#include <memory>

#include "src/api/index_buffer.hpp"
#include "src/api/rendering_pipeline_layout.hpp"
#include "src/api/shader.hpp"
#include "src/api/vertex_binding.hpp"

namespace api {
enum class DepthFunction {
  LESS
};
enum class CullMode {
  NONE,
  FRONT,
  BACK,
  FRONT_AND_BACK
};
enum class FrontFace {
  CW,
  CCW
};
struct RenderingPipelineLayoutConfig {
  CullMode cull_mode = CullMode::NONE;
  FrontFace front_face = FrontFace::CW;
  bool enable_depth_test = false;
  DepthFunction depth_function = DepthFunction::LESS;
};
class RenderingPipeline {
 protected:
  std::shared_ptr<VertexBinding> vertex_binding_;
  std::shared_ptr<IndexBuffer> index_buffer_;
  std::shared_ptr<Shader> vertex_shader_;
  std::shared_ptr<Shader> fragment_shader_;
  std::shared_ptr<api::RenderingPipelineLayout> pipeline_layout_;
  const api::RenderingPipelineLayoutConfig config_;
 public:
  RenderingPipeline(std::shared_ptr<VertexBinding> vertex_binding,
                    std::shared_ptr<IndexBuffer> index_buffer,
                    std::shared_ptr<Shader> vertex_shader,
                    std::shared_ptr<Shader> fragment_shader,
                    std::shared_ptr<api::RenderingPipelineLayout> pipeline_layout,
                    RenderingPipelineLayoutConfig config)
      : vertex_binding_(std::move(vertex_binding)),
        index_buffer_(std::move(index_buffer)),
        vertex_shader_(std::move(vertex_shader)),
        fragment_shader_(std::move(fragment_shader)),
        pipeline_layout_(std::move(pipeline_layout)),
        config_(config) {}

  virtual void Render() = 0;

  virtual void ViewportChanged() = 0;

  virtual ~RenderingPipeline() = default;
};
}

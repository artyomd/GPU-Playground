//
// Created by artyomd on 1/1/20.
//
#pragma once

#include "VertexBinding.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "RenderingPipelineLayout.h"

namespace api {
enum DepthFunction {
  LESS
};
enum CullMode {
  NONE,
  FRONT,
  BACK,
  FRONT_AND_BACK
};
enum FrontFace {
  CW,
  CCW
};
struct RenderingPipelineLayoutConfig {
  CullMode cull_mode_ = NONE;
  FrontFace front_face_ = FrontFace::CW;
  bool enable_depth_test_ = false;
  DepthFunction depth_function_ = DepthFunction::LESS;
};
class RenderingPipeline {
 protected:
  const VertexBinding *vertex_binding_;
  const IndexBuffer *index_buffer_;
  const Shader *vertex_shader_;
  const Shader *fragment_shader_;
  const api::RenderingPipelineLayout *pipeline_layout_;
  const api::RenderingPipelineLayoutConfig config_;
 public:
  RenderingPipeline(const VertexBinding *vertex_binding,
                    const IndexBuffer *index_buffer,
                    const Shader *vertex_shader,
                    const Shader *fragment_shader,
                    const RenderingPipelineLayout *pipeline_layout,
                    const RenderingPipelineLayoutConfig &rendering_pipeline_layout_config)
      : vertex_binding_(vertex_binding),
        index_buffer_(index_buffer),
        vertex_shader_(vertex_shader),
        fragment_shader_(fragment_shader),
        pipeline_layout_(pipeline_layout),
        config_(rendering_pipeline_layout_config) {}

  virtual void Render() = 0;

  virtual void ViewportChanged() = 0;

  virtual ~RenderingPipeline() = default;
};
}

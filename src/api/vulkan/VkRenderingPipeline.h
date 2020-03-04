//
// Created by artyomd on 3/4/20.
//

#pragma once

#include <api/RenderingPipeline.h>
#include "VkRenderingContext.h"

namespace api {
class VkRenderingPipeline : public RenderingPipeline {
 private:
  VkRenderingContext *context_;
  VkDevice *device_;
  VkPipeline pipeline_;
  VkPipelineLayout pipeline_layout_;
 public:
  VkRenderingPipeline(VkRenderingContext *context,
                      const VertexBinding *vertex_binding,
                      const IndexBuffer *index_buffer,
                      const Shader *vertex_shader,
                      const Shader *fragment_shader);

  void Render() override;
  virtual ~VkRenderingPipeline();
};
}
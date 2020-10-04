//
// Created by artyomd on 3/4/20.
//

#pragma once

#include <vector>

#include "src/api/RenderingPipeline.h"
#include "src/api/vulkan/VkRenderingContext.h"
#include "src/api/vulkan/VkRenderingPipelineLayout.h"

namespace api {
class VkRenderingPipeline : public RenderingPipeline {
 private:
  VkRenderingContext *context_;
  VkDevice *device_;
  VkPipeline pipeline_;

  void DestroyPipeline();
  void CreatePipeline();

 public:
  VkRenderingPipeline(VkRenderingContext *context,
                      const VertexBinding *vertex_binding,
                      const IndexBuffer *index_buffer,
                      const Shader *vertex_shader,
                      const Shader *fragment_shader,
                      const RenderingPipelineLayout *pipeline_layout,
                      const RenderingPipelineLayoutConfig &config = {});

  void Render() override;
  void ViewportChanged() override;
  ~VkRenderingPipeline() override;
};
}
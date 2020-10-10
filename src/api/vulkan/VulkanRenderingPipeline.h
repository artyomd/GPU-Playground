//
// Created by artyomd on 3/4/20.
//

#pragma once

#include <vulkan/vulkan.h>

#include "src/api/RenderingPipeline.h"
#include "src/api/vulkan/VulkanRenderingContext.h"
#include "src/api/vulkan/VulkanRenderingPipelineLayout.h"

namespace api {
class VulkanRenderingPipeline : public RenderingPipeline {
 private:
  std::shared_ptr<VulkanRenderingContext> context_;
  VkDevice device_;
  VkPipeline pipeline_{};

  void DestroyPipeline();
  void CreatePipeline();

 public:
  VulkanRenderingPipeline(std::shared_ptr<VulkanRenderingContext> context,
                          std::shared_ptr<VertexBinding> vertex_binding,
                          std::shared_ptr<IndexBuffer> index_buffer,
                          std::shared_ptr<Shader> vertex_shader,
                          std::shared_ptr<Shader> fragment_shader,
                          std::shared_ptr<RenderingPipelineLayout> pipeline_layout,
                          RenderingPipelineLayoutConfig config);

  void Render() override;
  void ViewportChanged() override;
  ~VulkanRenderingPipeline() override;
};
}
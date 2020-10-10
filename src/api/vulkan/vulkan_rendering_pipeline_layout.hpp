//
// Created by artyomd on 3/29/20.
//

#pragma once

#include "src/api/rendering_pipeline_layout.hpp"
#include "src/api/vulkan/vulkan_rendering_context.hpp"

namespace api::vulkan {
class VulkanRenderingPipelineLayout : public RenderingPipelineLayout {
 private:
  std::shared_ptr<VulkanRenderingContext> context_;
  VkDevice device_;
  VkDescriptorPool descriptor_pool_;
  int image_count_;
  std::vector<VkDescriptorSet> descriptor_sets_{};
  VkDescriptorSetLayout descriptor_set_layout_{};
  VkPipelineLayout pipeline_layout_ = nullptr;

 public:
  VulkanRenderingPipelineLayout(std::shared_ptr<VulkanRenderingContext> context,
                                std::vector<std::shared_ptr<Uniform>> bindings);
  [[nodiscard]] VkPipelineLayout GetPipelineLayout() const;
  void Bind() const override;
  ~VulkanRenderingPipelineLayout() override;
};
}
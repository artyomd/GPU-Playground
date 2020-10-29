//
// Created by artyomd on 12/30/19.
//

#pragma once

#include <vulkan/vulkan.h>

#include "src/api/vertex_buffer.hpp"
#include "src/api/vulkan/vulkan_rendering_context.hpp"

namespace api::vulkan {
class VulkanVertexBuffer : public VertexBuffer {
 private:
  std::shared_ptr<VulkanRenderingContext> context_;
  VkDevice device_;
  VkBuffer buffer_{};
  VkDeviceMemory memory_{};

  VkVertexInputBindingDescription vertex_input_binding_description_{};
  std::vector<VkVertexInputAttributeDescription> attribute_descriptions_{};
  VkPipelineVertexInputStateCreateInfo vertex_input_info_ = {};

 public:
  VulkanVertexBuffer(std::shared_ptr<VulkanRenderingContext> context,
                     size_t size_in_bytes,
                     const VertexBufferLayout& layout);
  void Update(void *data) override;

  [[nodiscard]] VkPipelineVertexInputStateCreateInfo GetVertexInputInfo() const;

  [[nodiscard]] const VkBuffer *GetBuffer() const;

  ~VulkanVertexBuffer() override;
};
}

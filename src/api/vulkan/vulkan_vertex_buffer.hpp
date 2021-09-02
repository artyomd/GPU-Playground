//
// Created by artyomd on 12/30/19.
//

#pragma once

#include <vulkan/vulkan.h>

#include "src/api/vertex_buffer.hpp"
#include "src/api/vulkan/vulkan_buffer.hpp"
#include "src/api/vulkan/vulkan_rendering_context.hpp"

namespace api::vulkan {
class VulkanVertexBuffer : public VulkanBuffer,
                           public VertexBuffer {

 public:
  VulkanVertexBuffer() = delete;
  VulkanVertexBuffer(const std::shared_ptr<VulkanRenderingContext>& context,
                     size_t size_in_bytes,
                     const VertexBufferLayout &layout);

  [[nodiscard]] VkPipelineVertexInputStateCreateInfo GetVertexInputInfo() const;

  void BindBuffer(const VkCommandBuffer &command_buffer);

  ~VulkanVertexBuffer() override = default;
 private:
  std::vector<VkVertexInputBindingDescription> vertex_input_binding_descriptions_{};
  std::vector<VkVertexInputAttributeDescription> attribute_descriptions_{};
  VkPipelineVertexInputStateCreateInfo vertex_input_info_ = {};
};
}

//
// Created by artyomd on 12/30/19.
//

#pragma once

#include <vulkan/vulkan.h>

#include "src/api/VertexBuffer.h"
#include "src/api/vulkan/VulkanRenderingContext.h"

namespace api {
class VulkanVertexBuffer : public VertexBuffer {
 private:
  std::shared_ptr<VulkanRenderingContext> context_;
  VkDevice device_;
  VkBuffer buffer_{};
  VkDeviceMemory memory_{};
 public:
  VulkanVertexBuffer(std::shared_ptr<VulkanRenderingContext> context, const void *vertices_data, VkDeviceSize buffer_size);

  void Bind() const override;

  void Unbind() const override;

  ~VulkanVertexBuffer() override;
};
}
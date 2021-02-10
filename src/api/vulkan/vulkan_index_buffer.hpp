//
// Created by artyomd on 2/25/20.
//

#pragma once

#include <vulkan/vulkan.h>

#include "src/api/index_buffer.hpp"
#include "src/api/vulkan/vulkan_rendering_context.hpp"
#include "src/api/vulkan/vulkan_buffer.hpp"

namespace api::vulkan {
class VulkanIndexBuffer :
    public VulkanBuffer,
    public IndexBuffer {
 public:
  VulkanIndexBuffer() = delete;
  VulkanIndexBuffer(const VulkanIndexBuffer &) = delete;
  VulkanIndexBuffer(std::shared_ptr<VulkanRenderingContext> context,
                    unsigned int item_count,
                    enum DataType type);
  [[nodiscard]] VkIndexType GetIndexType() const;
  ~VulkanIndexBuffer() override = default;
 private:
  VkIndexType index_type_;
};
}

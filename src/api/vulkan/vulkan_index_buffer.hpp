//
// Created by artyomd on 2/25/20.
//

#pragma once

#include <vulkan/vulkan.h>

#include "src/api/index_buffer.hpp"
#include "src/api/vulkan/vulkan_rendering_context.hpp"

namespace api::vulkan {
class VulkanIndexBuffer : public IndexBuffer {
 private:
  std::shared_ptr<VulkanRenderingContext> context_;
  VkDevice device_;

  VkBuffer buffer_ = nullptr;
  VkDeviceMemory memory_ = nullptr;
  VkIndexType index_type_;

 public:
  VulkanIndexBuffer(std::shared_ptr<VulkanRenderingContext> context,
                    unsigned int item_count,
                    enum DataType type);

 public:
  void Update(const void *data) override;
  [[nodiscard]] const VkBuffer *GetBuffer() const;
  [[nodiscard]] VkIndexType GetIndexType() const;
  ~VulkanIndexBuffer() override;
};
}

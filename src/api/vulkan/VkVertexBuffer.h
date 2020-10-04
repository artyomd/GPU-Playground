//
// Created by artyomd on 12/30/19.
//

#pragma once

#include <vulkan/vulkan.h>

#include "src/api/VertexBuffer.h"
#include "src/api/vulkan/VkRenderingContext.h"

namespace api {
class VkVertexBuffer : public VertexBuffer {
 private:
  VkRenderingContext *context_;
  VkDevice *device_;
  VkBuffer buffer_{};
  VkDeviceMemory memory_{};
 public:
  VkVertexBuffer(VkRenderingContext *context, const void *vertices_data, VkDeviceSize buffer_size);

  void Bind() const override;

  void Unbind() const override;

  ~VkVertexBuffer() override;
};
}
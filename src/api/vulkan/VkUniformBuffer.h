//
// Created by artyomd on 3/8/20.
//

#pragma once

#include "VkRenderingContext.h"
#include "VkUniform.h"

namespace api {
class VkUniformBuffer : public VkUniform {
 private:
  VkRenderingContext *context_;
  VkDescriptorPool *descriptor_pool_;
  VkDevice *device_;
  int length_;
  std::vector<VkBuffer> uniform_buffers_{};
  std::vector<VkDescriptorBufferInfo> descriptor_buffer_info_{};
  std::vector<VkDeviceMemory> uniform_buffers_memory_{};
 public:
  VkUniformBuffer(VkRenderingContext *context, int length, int binding_point, ShaderType shader_stage);
  void Update(const void *data) override;
  [[nodiscard]] VkDescriptorSetLayoutBinding GetLayoutBinding() const override;
  [[nodiscard]] VkWriteDescriptorSet GetWriteDescriptorSetFor(int image_index) const override;
  ~VkUniformBuffer() override;
};
}
//
// Created by artyomd on 3/8/20.
//

#pragma once

#include <api/UniformBuffer.h>
#include "VkRenderingContext.h"

namespace api {
class VkUniformBuffer : public UniformBuffer {
 private:
  VkRenderingContext *context_;
  VkDescriptorPool *descriptor_pool_;
  VkDevice *device_;
  int image_count_;
  std::vector<VkDescriptorSet> descriptor_sets_{};
  std::vector<VkBuffer> uniform_buffers_{};
  std::vector<VkDeviceMemory> uniform_buffers_memory_{};
  VkDescriptorSetLayout descriptor_set_layout_;
  VkPipelineLayout *pipeline_layout_;
 public:
  VkUniformBuffer(VkRenderingContext *context, int length, int binding_point, ShaderType shader_stage);
  VkPipelineLayout *GetPipelineLayout() const;
  void Update(const void *data) override;
  void Bind() const override;
  ~VkUniformBuffer() override;
};
}
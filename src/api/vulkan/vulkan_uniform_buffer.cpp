//
// Created by artyomd on 3/8/20.
//

#include "src/api/vulkan/vulkan_uniform_buffer.hpp"

#include <cstring>

#include "src/api/vulkan/vulkan_utils.hpp"

api::vulkan::VulkanUniformBuffer::VulkanUniformBuffer(std::shared_ptr<VulkanRenderingContext> context,
                                                      size_t size_in_bytes,
                                                      int binding_point,
                                                      api::ShaderType shader_stage)
    : UniformBuffer(binding_point, shader_stage),
      context_(std::move(context)),
      device_(context_->GetDevice()),
      size_in_bytes_(size_in_bytes) {

  auto image_count = context_->GetImageCount();
  uniform_buffers_.resize(image_count);
  uniform_buffers_memory_.resize(image_count);
  descriptor_buffer_info_.resize(image_count);

  for (unsigned int i = 0; i < uniform_buffers_.size(); i++) {
    context_->CreateBuffer(size_in_bytes,
                           VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                           &uniform_buffers_[i],
                           &uniform_buffers_memory_[i]);

    descriptor_buffer_info_[i].buffer = uniform_buffers_[i];
    descriptor_buffer_info_[i].offset = 0;
    descriptor_buffer_info_[i].range = size_in_bytes;
  }
}

VkWriteDescriptorSet api::vulkan::VulkanUniformBuffer::GetWriteDescriptorSetFor(int image_index) const {
  VkWriteDescriptorSet write_descriptor_set{};
  write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  write_descriptor_set.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  write_descriptor_set.descriptorCount = 1;
  write_descriptor_set.pBufferInfo = &descriptor_buffer_info_[image_index];
  write_descriptor_set.dstBinding = this->binding_point_;
  write_descriptor_set.dstArrayElement = 0;
  return write_descriptor_set;
}

void api::vulkan::VulkanUniformBuffer::Update(const void *data) {
  int current_image = context_->GetCurrentImageIndex();
  void *mapped_data;
  vkMapMemory(device_, uniform_buffers_memory_[current_image], 0, size_in_bytes_, 0, &mapped_data);
  memcpy(mapped_data, data, size_in_bytes_);
  vkUnmapMemory(device_, uniform_buffers_memory_[current_image]);
}

api::vulkan::VulkanUniformBuffer::~VulkanUniformBuffer() {
  for (auto &uniform_buffer : uniform_buffers_) {
    vkDestroyBuffer(device_, uniform_buffer, nullptr);
  }
  for (auto &i : uniform_buffers_memory_) {
    vkFreeMemory(device_, i, nullptr);
  }
}

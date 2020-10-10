//
// Created by artyomd on 3/8/20.
//

#include "src/api/vulkan/vulkan_uniform_buffer.hpp"

#include <cstring>

#include "src/api/vulkan/vulkan_utils.hpp"

api::vulkan::VulkanUniformBuffer::VulkanUniformBuffer(std::shared_ptr<VulkanRenderingContext> context,
                                                      int length, int binding_point, api::ShaderType shader_stage)
    : UniformBuffer(binding_point, shader_stage),
      context_(std::move(context)),
      device_(context_->GetDevice()),
      length_(length) {

  auto image_count = context_->GetImageCount();
  uniform_buffers_.resize(image_count);
  uniform_buffers_memory_.resize(image_count);
  descriptor_buffer_info_.resize(image_count);
  for (size_t i = 0; i < uniform_buffers_.size(); i++) {
    context_->CreateBuffer(length_, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                           &uniform_buffers_[i],
                           &uniform_buffers_memory_[i]);

    descriptor_buffer_info_[i].buffer = uniform_buffers_[i];
    descriptor_buffer_info_[i].offset = 0;
    descriptor_buffer_info_[i].range = length_;
  }

}

VkDescriptorSetLayoutBinding api::vulkan::VulkanUniformBuffer::GetLayoutBinding() const {
  VkDescriptorSetLayoutBinding descriptor_set_layout_binding{};
  descriptor_set_layout_binding.binding = binding_point_;
  descriptor_set_layout_binding.descriptorCount = 1;
  descriptor_set_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  descriptor_set_layout_binding.pImmutableSamplers = nullptr;
  descriptor_set_layout_binding.stageFlags = GetVkShaderStageFlag(shader_stage_);
  return descriptor_set_layout_binding;
}

VkWriteDescriptorSet api::vulkan::VulkanUniformBuffer::GetWriteDescriptorSetFor(int image_index) const {
  VkWriteDescriptorSet descriptor_writes = {};
  descriptor_writes.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  descriptor_writes.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  descriptor_writes.descriptorCount = 1;
  descriptor_writes.pBufferInfo = &descriptor_buffer_info_[image_index];
  return descriptor_writes;
}

void api::vulkan::VulkanUniformBuffer::Update(const void *data) {
  int current_image = context_->GetCurrentImageIndex();
  void *buffer_data;
  vkMapMemory(device_, uniform_buffers_memory_[current_image], 0, length_, 0, &buffer_data);
  memcpy(buffer_data, data, length_);
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

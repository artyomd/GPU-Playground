//
// Created by artyomd on 3/8/20.
//

#include <array>
#include <cstring>
#include "VkUniformBuffer.h"

VkShaderStageFlags GetVkShaderStageFlag(api::ShaderType shader_type) {
  switch (shader_type) {
    case api::SHADER_TYPE_VERTEX:return VK_SHADER_STAGE_VERTEX_BIT;
    case api::SHADER_TYPE_FRAGMENT:return VK_SHADER_STAGE_FRAGMENT_BIT;
  }
}
api::VkUniformBuffer::VkUniformBuffer(VkRenderingContext *context,
                                      int length, int binding_point, api::ShaderType shader_stage)
    : UniformBuffer(length, binding_point, shader_stage),
      context_(context),
      device_(context->GetDevice()),
      descriptor_pool_(context->GetDescriptorPool()),
      image_count_(context->GetImageCount()) {
  VkDescriptorSetLayoutBinding descriptor_set_layout_binding{};
  descriptor_set_layout_binding.binding = binding_location_;
  descriptor_set_layout_binding.descriptorCount = 1;
  descriptor_set_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  descriptor_set_layout_binding.pImmutableSamplers = nullptr;
  descriptor_set_layout_binding.stageFlags = GetVkShaderStageFlag(shader_stage);

  VkDescriptorSetLayoutCreateInfo layout_info = {};
  layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layout_info.bindingCount = 1;
  layout_info.pBindings = &descriptor_set_layout_binding;

  if (vkCreateDescriptorSetLayout(*device_, &layout_info, nullptr, &descriptor_set_layout_)!=VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor set layout!");
  }

  VkPipelineLayoutCreateInfo pipeline_layout_info = {};
  pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipeline_layout_info.setLayoutCount = 1;
  pipeline_layout_info.pSetLayouts = &descriptor_set_layout_;
  if (vkCreatePipelineLayout(*device_, &pipeline_layout_info, nullptr, pipeline_layout_)!=VK_SUCCESS) {
    throw std::runtime_error("failed to create pipeline layout!");
  }

  uniform_buffers_.resize(image_count_);
  uniform_buffers_memory_.resize(image_count_);
  for (size_t i = 0; i < uniform_buffers_.size(); i++) {
    context_->CreateBuffer(length_, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                           uniform_buffers_[i],
                           uniform_buffers_memory_[i]);
  }

  std::vector<VkDescriptorSetLayout> layouts(image_count_, descriptor_set_layout_);
  VkDescriptorSetAllocateInfo alloc_info = {};
  alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  alloc_info.descriptorPool = *descriptor_pool_;
  alloc_info.descriptorSetCount = static_cast<uint32_t>(image_count_);
  alloc_info.pSetLayouts = layouts.data();
  descriptor_sets_.resize(image_count_);
  if (vkAllocateDescriptorSets(*device_, &alloc_info, descriptor_sets_.data())!=VK_SUCCESS) {
    throw std::runtime_error("failed to allocate descriptor sets!");
  }

  for (size_t i = 0; i < image_count_; i++) {
    VkDescriptorBufferInfo buffer_info = {};
    buffer_info.buffer = uniform_buffers_[i];
    buffer_info.offset = 0;
    buffer_info.range = length_;

    std::array<VkWriteDescriptorSet, 1> descriptor_writes = {};
    descriptor_writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptor_writes[0].dstSet = descriptor_sets_[i];
    descriptor_writes[0].dstBinding = 0;
    descriptor_writes[0].dstArrayElement = 0;
    descriptor_writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptor_writes[0].descriptorCount = 1;
    descriptor_writes[0].pBufferInfo = &buffer_info;
    vkUpdateDescriptorSets(*device_, static_cast<uint32_t>(descriptor_writes.size()),
                           descriptor_writes.data(), 0, nullptr);
  }
}

void api::VkUniformBuffer::Update(const void *data) {
  int current_image = context_->GetCurrentImageIndex();
  void *buffer_data;
  vkMapMemory(*device_, uniform_buffers_memory_[current_image], 0, length_, 0, &buffer_data);
  memcpy(buffer_data, data, length_);
  vkUnmapMemory(*device_, uniform_buffers_memory_[current_image]);

}

void api::VkUniformBuffer::Bind() const {
  vkCmdBindDescriptorSets(*context_->GetCurrentCommandBuffer(),
                          VK_PIPELINE_BIND_POINT_GRAPHICS,
                          *pipeline_layout_,
                          0,
                          1,
                          &descriptor_sets_[context_->GetCurrentImageIndex()],
                          0,
                          nullptr);
}

VkPipelineLayout *api::VkUniformBuffer::GetPipelineLayout() const {
  return pipeline_layout_;
}

api::VkUniformBuffer::~VkUniformBuffer() {
  for (auto &uniform_buffer : uniform_buffers_) {
    vkDestroyBuffer(*device_, uniform_buffer, nullptr);
  }
  for (auto &i : uniform_buffers_memory_) {
    vkFreeMemory(*device_, i, nullptr);
  }
  vkDestroyDescriptorSetLayout(*device_, descriptor_set_layout_, nullptr);
  vkFreeDescriptorSets(*device_, *descriptor_pool_, descriptor_sets_.size(), descriptor_sets_.data());
  vkDestroyPipelineLayout(*device_, *pipeline_layout_, nullptr);
}

//
// Created by artyomd on 3/29/20.
//

#include "src/api/vulkan/vulkan_rendering_pipeline_layout.hpp"

#include <vulkan/vulkan.h>
#include <stdexcept>
#include <utility>

#include "src/api/vulkan/vulkan_uniform.hpp"

api::vulkan::VulkanRenderingPipelineLayout::VulkanRenderingPipelineLayout(
    std::shared_ptr<VulkanRenderingContext> context,
    std::vector<std::shared_ptr<Uniform>> bindings)
    :
    RenderingPipelineLayout(std::move(bindings)),
    context_(std::move(context)),
    device_(context_->GetDevice()),
    descriptor_pool_(context_->GetDescriptorPool()),
    image_count_(context_->GetImageCount()) {

  std::vector<VkDescriptorSetLayoutBinding> p_bindings{};
  for (const auto &value: bindings_) {
    p_bindings.push_back(std::dynamic_pointer_cast<VulkanUniform>(value)->GetLayoutBinding());
  }

  VkDescriptorSetLayoutCreateInfo layout_info = {};
  layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layout_info.bindingCount = p_bindings.size();
  layout_info.pBindings = p_bindings.data();

  if (vkCreateDescriptorSetLayout(device_, &layout_info, nullptr, &descriptor_set_layout_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor set layout!");
  }

  VkPipelineLayoutCreateInfo pipeline_layout_info = {};
  pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipeline_layout_info.setLayoutCount = 1;
  pipeline_layout_info.pSetLayouts = &descriptor_set_layout_;
  if (vkCreatePipelineLayout(device_, &pipeline_layout_info, nullptr, &pipeline_layout_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create pipeline layout!");
  }

  std::vector<VkDescriptorSetLayout> layouts(image_count_, descriptor_set_layout_);
  VkDescriptorSetAllocateInfo alloc_info = {};
  alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  alloc_info.descriptorPool = descriptor_pool_;
  alloc_info.descriptorSetCount = static_cast<uint32_t>(image_count_);
  alloc_info.pSetLayouts = layouts.data();
  descriptor_sets_.resize(image_count_);
  if (vkAllocateDescriptorSets(device_, &alloc_info, descriptor_sets_.data()) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate descriptor sets!");
  }

  for (size_t i = 0; i < image_count_; i++) {
    std::vector<VkWriteDescriptorSet> descriptor_writes{};
    for (int j = 0; j < bindings_.size(); j++) {
      auto descriptor_write = std::dynamic_pointer_cast<VulkanUniform>(bindings_[j])->GetWriteDescriptorSetFor(i);
      descriptor_write.dstSet = descriptor_sets_[i];
      descriptor_write.dstBinding = j;
      descriptor_write.dstArrayElement = 0;
      descriptor_writes.push_back(descriptor_write);
    }
    vkUpdateDescriptorSets(device_,
                           static_cast<uint32_t>(descriptor_writes.size()),
                           descriptor_writes.data(), 0, nullptr);
  }
}

VkPipelineLayout api::vulkan::VulkanRenderingPipelineLayout::GetPipelineLayout() const {
  return pipeline_layout_;
}

void api::vulkan::VulkanRenderingPipelineLayout::Bind() const {
  vkCmdBindDescriptorSets(context_->GetCurrentCommandBuffer(),
                          VK_PIPELINE_BIND_POINT_GRAPHICS,
                          pipeline_layout_,
                          0,
                          1,
                          &descriptor_sets_[context_->GetCurrentImageIndex()],
                          0,
                          nullptr);
}

api::vulkan::VulkanRenderingPipelineLayout::~VulkanRenderingPipelineLayout() {
  vkDestroyDescriptorSetLayout(device_, descriptor_set_layout_, nullptr);
  vkFreeDescriptorSets(device_, descriptor_pool_, descriptor_sets_.size(), descriptor_sets_.data());
  vkDestroyPipelineLayout(device_, pipeline_layout_, nullptr);
}

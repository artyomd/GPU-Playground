//
// Created by artyomd on 3/29/20.
//

#include <vulkan/vulkan.h>
#include <stdexcept>
#include "VkRenderingPipelineLayout.h"
#include "VkUniform.h"
api::VkRenderingPipelineLayout::VkRenderingPipelineLayout(VkRenderingContext *context,
                                                          const std::vector<Uniform *> &bindings) :
    RenderingPipelineLayout(bindings),
    context_(context),
    device_(context->GetDevice()),
    descriptor_pool_(context->GetDescriptorPool()), image_count_(context->GetImageCount()) {

  std::vector<VkDescriptorSetLayoutBinding> p_bindings{};
  for (auto value: bindings_) {
    p_bindings.push_back(dynamic_cast<VkUniform *>(value)->GetLayoutBinding());
  }

  VkDescriptorSetLayoutCreateInfo layout_info = {};
  layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layout_info.bindingCount = 1;
  layout_info.pBindings = p_bindings.data();

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
    std::vector<VkWriteDescriptorSet> descriptor_writes{};
    for (auto value: bindings_) {
      auto descriptor_write = dynamic_cast<VkUniform *>(value)->GetWriteDescriptorSetFor(i);
      descriptor_write.dstSet = descriptor_sets_[i];
      descriptor_writes.push_back(descriptor_write);
    }
    vkUpdateDescriptorSets(*device_,
                           static_cast<uint32_t>(descriptor_writes.size()),
                           descriptor_writes.data(), 0, nullptr);
  }
}

VkPipelineLayout *api::VkRenderingPipelineLayout::GetPipelineLayout() const {
  return pipeline_layout_;
}

void api::VkRenderingPipelineLayout::Bind() const {
  vkCmdBindDescriptorSets(*context_->GetCurrentCommandBuffer(),
                          VK_PIPELINE_BIND_POINT_GRAPHICS,
                          *pipeline_layout_,
                          0,
                          1,
                          &descriptor_sets_[context_->GetCurrentImageIndex()],
                          0,
                          nullptr);
}

api::VkRenderingPipelineLayout::~VkRenderingPipelineLayout() {
  vkDestroyDescriptorSetLayout(*device_, descriptor_set_layout_, nullptr);
  vkFreeDescriptorSets(*device_, *descriptor_pool_, descriptor_sets_.size(), descriptor_sets_.data());
  vkDestroyPipelineLayout(*device_, *pipeline_layout_, nullptr);
}

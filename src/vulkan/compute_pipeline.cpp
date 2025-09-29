#include "compute_pipeline.hpp"

#include <magic_enum/magic_enum.hpp>
#include <spdlog/spdlog.h>

#include <ranges>

#include "utils.hpp"

std::shared_ptr<vulkan::ComputePipeline> vulkan::ComputePipeline::Create(
    const std::shared_ptr<RenderingContext> &context, const std::shared_ptr<Shader> &compute_shader,
    const size_t descriptor_set_count) {
  return std::shared_ptr<ComputePipeline>(new ComputePipeline(context, compute_shader, descriptor_set_count));
}

vulkan::ComputePipeline::ComputePipeline(const std::shared_ptr<RenderingContext> &context,
                                         const std::shared_ptr<Shader> &compute_shader, size_t descriptor_set_count)
    : context_(context), compute_shader_(compute_shader) {
  const auto shader_stage_info = compute_shader_->GetShaderStageInfo();
  auto shader_binding = compute_shader_->GetBindings();
  std::vector<VkDescriptorSetLayoutBinding> pipeline_bindings;
  const auto get_descriptor_layout = [](const SpvReflectDescriptorBinding &binding,
                                        const VkShaderStageFlags &shader_stage) {
    VkDescriptorSetLayoutBinding descriptor_set_layout_binding = {
        .binding = binding.binding,
        .descriptorType = static_cast<VkDescriptorType>(binding.descriptor_type),
        .descriptorCount = 1,
        .stageFlags = shader_stage,
    };
    for (uint32_t i_dim = 0; i_dim < binding.array.dims_count; ++i_dim) {
      descriptor_set_layout_binding.descriptorCount *= binding.array.dims[i_dim];
    }
    return descriptor_set_layout_binding;
  };
  for (const auto &value : std::views::values(shader_binding)) {
    pipeline_bindings.emplace_back(get_descriptor_layout(value, shader_stage_info.stage));
  }

  const VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info{
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
      .bindingCount = static_cast<uint32_t>(pipeline_bindings.size()),
      .pBindings = pipeline_bindings.data(),
  };

  VK_CALL(vkCreateDescriptorSetLayout(context_->GetDevice(), &descriptor_set_layout_create_info, nullptr, &layout_));

  const VkPipelineLayoutCreateInfo pipeline_layout_info = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
      .setLayoutCount = 1,
      .pSetLayouts = &layout_,
  };

  VK_CALL(vkCreatePipelineLayout(context_->GetDevice(), &pipeline_layout_info, nullptr, &pipeline_layout_));

  const VkComputePipelineCreateInfo pipeline_create_info{
      .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
      .stage = shader_stage_info,
      .layout = pipeline_layout_,
  };
  VK_CALL(
      vkCreateComputePipelines(context_->GetDevice(), VK_NULL_HANDLE, 1, &pipeline_create_info, nullptr, &pipeline_));

  descriptor_sets_.resize(descriptor_set_count);
  for (auto i = 0u; i < descriptor_set_count; i++) {
    const VkDescriptorSetAllocateInfo alloc_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = context_->GetDescriptorPool(),
        .descriptorSetCount = 1,
        .pSetLayouts = &layout_,
    };
    VK_CALL(vkAllocateDescriptorSets(context_->GetDevice(), &alloc_info, &descriptor_sets_[i].descriptor_set));
  }
}

void vulkan::ComputePipeline::SetBuffer(uint32_t binding_point, const uint32_t descriptor_set_index,
                                        const std::shared_ptr<Buffer> &buffer) {
  if (descriptor_sets_.size() <= descriptor_set_index) {
    throw std::runtime_error("invalid descriptor set index");
  }
  const auto bindings = compute_shader_->GetBindings();
  if (!bindings.contains(binding_point)) {
    spdlog::warn("warning: entry point {} does not exists in shaders", binding_point);
    return;
  }
  const auto descriptor_set_layout = bindings.at(binding_point);
  const auto descriptor_type = static_cast<VkDescriptorType>(descriptor_set_layout.descriptor_type);
  if (descriptor_type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
    descriptor_sets_[descriptor_set_index].uniform_buffers[binding_point] = buffer;
  } else if (descriptor_type == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER) {
    descriptor_sets_[descriptor_set_index].storage_buffers[binding_point] = buffer;
  } else {
    spdlog::warn("unhandled buffer type", magic_enum::enum_name(descriptor_type));
  }

  const VkDescriptorBufferInfo buffer_info = {
      .buffer = buffer->GetBuffer(),
      .offset = 0,
      .range = buffer->GetSizeInBytes(),
  };
  const VkWriteDescriptorSet descriptor_write = {
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .pNext = nullptr,
      .dstSet = descriptor_sets_[descriptor_set_index].descriptor_set,
      .dstBinding = binding_point,
      .dstArrayElement = 0,
      .descriptorCount = 1,
      .descriptorType = descriptor_type,
      .pImageInfo = VK_NULL_HANDLE,
      .pBufferInfo = &buffer_info,
      .pTexelBufferView = VK_NULL_HANDLE,
  };
  vkUpdateDescriptorSets(context_->GetDevice(), 1, &descriptor_write, 0, nullptr);
}

void vulkan::ComputePipeline::SetImage(uint32_t binding_point, const uint32_t descriptor_set_index,
                                       const std::shared_ptr<ImageView> &image) {
  if (descriptor_sets_.size() <= descriptor_set_index) {
    throw std::runtime_error("invalid descriptor set index");
  }
  const auto bindings = compute_shader_->GetBindings();
  if (!bindings.contains(binding_point)) {
    spdlog::warn("warning: entry point {} does not exists in shaders", binding_point);
    return;
  }
  const auto descriptor_set_layout = bindings.at(binding_point);
  const auto descriptor_type = static_cast<VkDescriptorType>(descriptor_set_layout.descriptor_type);
  if (descriptor_type == VK_DESCRIPTOR_TYPE_STORAGE_IMAGE) {
    descriptor_sets_[descriptor_set_index].storage_images[binding_point] = image;
  } else {
    spdlog::warn("unhandled buffer type", magic_enum::enum_name(descriptor_type));
  }

  const VkDescriptorImageInfo image_info{
      .sampler = VK_NULL_HANDLE,
      .imageView = image->GetImageView(),
      .imageLayout = VK_IMAGE_LAYOUT_GENERAL,
  };
  const VkWriteDescriptorSet descriptor_write{
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .pNext = nullptr,
      .dstSet = descriptor_sets_[descriptor_set_index].descriptor_set,
      .dstBinding = binding_point,
      .dstArrayElement = 0,
      .descriptorCount = 1,
      .descriptorType = descriptor_type,
      .pImageInfo = &image_info,
      .pBufferInfo = VK_NULL_HANDLE,
      .pTexelBufferView = VK_NULL_HANDLE,
  };
  vkUpdateDescriptorSets(context_->GetDevice(), 1, &descriptor_write, 0, nullptr);
}

size_t vulkan::ComputePipeline::GetDescriptorSetCount() const { return descriptor_sets_.size(); }

void vulkan::ComputePipeline::Dispatch(const VkCommandBuffer &command_buffer, const uint32_t &group_count_x,
                                       const uint32_t &group_count_y, const uint32_t &group_count_z,
                                       const size_t &descriptor_index) const {
  vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline_);
  vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline_layout_, 0, 1,
                          &descriptor_sets_[descriptor_index].descriptor_set, 0, nullptr);
  vkCmdDispatch(command_buffer, group_count_x, group_count_y, group_count_z);
}

vulkan::ComputePipeline::~ComputePipeline() {
  for (const auto &descriptor_set : descriptor_sets_) {
    vkFreeDescriptorSets(context_->GetDevice(), context_->GetDescriptorPool(), 1, &descriptor_set.descriptor_set);
  }
  vkDestroyDescriptorSetLayout(context_->GetDevice(), layout_, nullptr);
  vkDestroyPipelineLayout(context_->GetDevice(), pipeline_layout_, nullptr);
  vkDestroyPipeline(context_->GetDevice(), pipeline_, nullptr);
}

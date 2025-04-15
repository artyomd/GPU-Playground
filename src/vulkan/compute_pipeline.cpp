#include "compute_pipeline.hpp"

#include <spdlog/spdlog.h>
#include <sys/socket.h>

#include <magic_enum/magic_enum.hpp>

#include "utils.hpp"

std::shared_ptr<vulkan::ComputePipeline> vulkan::ComputePipeline::Create(
    const std::shared_ptr<RenderingContext> &context, const std::shared_ptr<Shader> &compute_shader,
    const size_t descriptor_set_count) {
  return std::shared_ptr<ComputePipeline>(new ComputePipeline(context, compute_shader, descriptor_set_count));
}

vulkan::ComputePipeline::ComputePipeline(const std::shared_ptr<RenderingContext> &context,
                                         const std::shared_ptr<Shader> &compute_shader, size_t descriptor_set_count)
    : context_(context), compute_shader_(compute_shader) {
  auto pipeline_bindings = compute_shader_->GetBindings();

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

  const auto shader_stage_info = compute_shader_->GetShaderStageInfo();
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
  auto get_descriptor_type = [&binding_point,
                              &buffer](const std::shared_ptr<Shader> &shader) -> std::optional<VkDescriptorType> {
    for (const auto binding : shader->GetBindings()) {
      if (binding.binding == binding_point && (binding.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER ||
                                               binding.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)) {
        if (binding.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
          if (auto expected_size = shader->DescriptorSizeInBytes(binding_point);
              expected_size != buffer->GetSizeInBytes()) {
            spdlog::warn("uniform buffer size mismatch expected:{} found:{}", expected_size, buffer->GetSizeInBytes());
          }
        }
        return binding.descriptorType;
      }
    }
    return {};
  };
  const auto binding_type = get_descriptor_type(compute_shader_);
  if (!binding_type.has_value()) {
    spdlog::warn("warning: entry point {} does not exists in shaders", binding_point);
    return;
  }
  if (binding_type.value() == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
    descriptor_sets_[descriptor_set_index].uniform_buffers[binding_point] = buffer;
  } else if (binding_type.value() == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER) {
    descriptor_sets_[descriptor_set_index].storage_buffers[binding_point] = buffer;
  } else {
    spdlog::warn("unhandled buffer type", magic_enum::enum_name(binding_type.value()));
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
      .descriptorType = binding_type.value(),
      .pImageInfo = VK_NULL_HANDLE,
      .pBufferInfo = &buffer_info,
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

#include "rendering_pipeline.hpp"

#include <spdlog/spdlog.h>

#include <algorithm>
#include <array>
#include <ranges>
#include <utility>

#include "utils.hpp"

namespace vulkan {
VkFormat GetVkFormat(const DataType type, const uint32_t count) {
  switch (type) {
    case DataType::BYTE:
      switch (count) {
        case 1:
          return VK_FORMAT_R8_UNORM;
        case 2:
          return VK_FORMAT_R8G8_UNORM;
        case 3:
          return VK_FORMAT_R8G8B8_UNORM;
        case 4:
          return VK_FORMAT_R8G8B8A8_UNORM;
        default:
          throw std::runtime_error("unsupported count");
      }
    case DataType::UINT_8:
      switch (count) {
        case 1:
          return VK_FORMAT_R8_UINT;
        case 2:
          return VK_FORMAT_R8G8_UINT;
        case 3:
          return VK_FORMAT_R8G8B8_UINT;
        case 4:
          return VK_FORMAT_R8G8B8A8_UINT;
        default:
          throw std::runtime_error("unsupported count");
      }
    case DataType::UINT_16:
      switch (count) {
        case 1:
          return VK_FORMAT_R16_UINT;
        case 2:
          return VK_FORMAT_R16G16_UINT;
        case 3:
          return VK_FORMAT_R16G16B16_UINT;
        case 4:
          return VK_FORMAT_R16G16B16A16_UINT;
        default:
          throw std::runtime_error("unsupported count");
      }
    case DataType::UINT_32:
      switch (count) {
        case 1:
          return VK_FORMAT_R32_UINT;
        case 2:
          return VK_FORMAT_R32G32_UINT;
        case 3:
          return VK_FORMAT_R32G32B32_UINT;
        case 4:
          return VK_FORMAT_R32G32B32A32_UINT;
        default:
          throw std::runtime_error("unsupported count");
      }
    case DataType::FLOAT:
      switch (count) {
        case 1:
          return VK_FORMAT_R32_SFLOAT;
        case 2:
          return VK_FORMAT_R32G32_SFLOAT;
        case 3:
          return VK_FORMAT_R32G32B32_SFLOAT;
        case 4:
          return VK_FORMAT_R32G32B32A32_SFLOAT;
        default:
          throw std::runtime_error("unsupported count");
      }
    default:
      throw std::runtime_error("unsupported enum");
  }
}
}  // namespace vulkan

std::shared_ptr<vulkan::RenderingPipeline> vulkan::RenderingPipeline::Create(
    const std::shared_ptr<RenderingContext> &context, const std::shared_ptr<RenderPass> &render_pass,
    const std::shared_ptr<Shader> &vertex_shader, const std::shared_ptr<Shader> &fragment_shader,
    const VertexBufferLayout &vbl, const RenderingPipelineConfig &config, const size_t &descriptor_set_count) {
  return Create(context, render_pass, vertex_shader, fragment_shader, {{0, vbl}}, config, descriptor_set_count);
}
std::shared_ptr<vulkan::RenderingPipeline> vulkan::RenderingPipeline::Create(
    const std::shared_ptr<RenderingContext> &context, const std::shared_ptr<RenderPass> &render_pass,
    const std::shared_ptr<Shader> &vertex_shader, const std::shared_ptr<Shader> &fragment_shader,
    const std::map<uint32_t, VertexBufferLayout> &vbl_map, const RenderingPipelineConfig &config,
    const size_t &descriptor_set_count) {
  return std::shared_ptr<RenderingPipeline>(new RenderingPipeline(context, render_pass, vertex_shader, fragment_shader,
                                                                  vbl_map, config, descriptor_set_count));
}

vulkan::RenderingPipeline::RenderingPipeline(const std::shared_ptr<RenderingContext> &context,
                                             const std::shared_ptr<RenderPass> &render_pass,
                                             const std::shared_ptr<Shader> &vertex_shader,
                                             const std::shared_ptr<Shader> &fragment_shader,
                                             const std::map<uint32_t, VertexBufferLayout> &vbl_map,
                                             const RenderingPipelineConfig &config, size_t descriptor_set_count)
    : context_(context), render_pass_(render_pass), vertex_shader_(vertex_shader), fragment_shader_(fragment_shader) {
  std::array shader_stages = {vertex_shader_->GetShaderStageInfo(), fragment_shader_->GetShaderStageInfo()};
  auto vertex_bindings = vertex_shader_->GetBindings();
  auto fragment_bindings = fragment_shader_->GetBindings();
  std::vector<VkDescriptorSetLayoutBinding> pipeline_bindings;
  pipeline_bindings.reserve(pipeline_bindings.size() + fragment_bindings.size());
  const auto get_descriptor_layout = [](const SpvReflectDescriptorBinding &binding, VkShaderStageFlags shader_stage) {
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
  for (const auto &value : std::views::values(vertex_bindings)) {
    pipeline_bindings.emplace_back(get_descriptor_layout(value, shader_stages[0].stage));
  }
  for (const auto &value : std::views::values(fragment_bindings)) {
    pipeline_bindings.emplace_back(get_descriptor_layout(value, shader_stages[1].stage));
  }
  const VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info{
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
      .bindingCount = static_cast<uint32_t>(pipeline_bindings.size()),
      .pBindings = pipeline_bindings.data(),
  };

  VK_CALL(vkCreateDescriptorSetLayout(context_->GetDevice(), &descriptor_set_layout_create_info, nullptr, &layout_));

  const VkPipelineInputAssemblyStateCreateInfo input_assembly = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
      .topology = config.draw_mode,
      .primitiveRestartEnable = VK_FALSE,
  };

  constexpr VkPipelineViewportStateCreateInfo viewport_state = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
      .viewportCount = 1,
      .pViewports = VK_NULL_HANDLE,
      .scissorCount = 1,
      .pScissors = VK_NULL_HANDLE,
  };

  const VkPipelineRasterizationStateCreateInfo rasterizer = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
      .depthClampEnable = VK_FALSE,
      .rasterizerDiscardEnable = VK_FALSE,
      .polygonMode = VK_POLYGON_MODE_FILL,
      .cullMode = config.cull_mode,
      .frontFace = config.front_face,
      .depthBiasEnable = VK_FALSE,
      .lineWidth = 1.0f,
  };

  const VkPipelineMultisampleStateCreateInfo multisampling = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
      .rasterizationSamples = config.sample_count,
      .sampleShadingEnable = VK_FALSE,
      .alphaToCoverageEnable = VK_FALSE,
  };

  const VkPipelineColorBlendStateCreateInfo color_blending = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
      .logicOpEnable = VK_FALSE,
      .logicOp = VK_LOGIC_OP_COPY,
      .attachmentCount = 1,
      .pAttachments = &config.color_blend_attachment_state,
      .blendConstants = {0.0f, 0.0f, 0.0f, 0.0f},
  };

  const VkPipelineDepthStencilStateCreateInfo depth_stencil = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
      .depthTestEnable = config.enable_depth_test,
      .depthWriteEnable = VK_TRUE,
      .depthCompareOp = config.depth_function,
      .depthBoundsTestEnable = VK_FALSE,
      .minDepthBounds = 0.0F,
      .maxDepthBounds = 1.0F,
  };

  const VkPipelineLayoutCreateInfo pipeline_layout_info = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
      .setLayoutCount = 1,
      .pSetLayouts = &layout_,
  };

  VK_CALL(vkCreatePipelineLayout(context_->GetDevice(), &pipeline_layout_info, nullptr, &pipeline_layout_));

  const std::vector dynamic_states = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
  const VkPipelineDynamicStateCreateInfo dynamic_state_create_info{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
      .dynamicStateCount = static_cast<uint32_t>(dynamic_states.size()),
      .pDynamicStates = dynamic_states.data(),
  };

  std::vector<VkVertexInputBindingDescription> vertex_input_binding_descriptions;
  std::vector<VkVertexInputAttributeDescription> attribute_descriptions;
  for (const auto &[vertex_buffer_binding_point, vbl] : vbl_map) {
    const auto &elements = vbl.GetElements();
    const auto stride = static_cast<uint32_t>(vbl.GetElementSize());
    size_t offset = 0;
    for (auto [binding_index, type, count] : elements) {
      VkVertexInputAttributeDescription description{
          .location = binding_index,
          .binding = vertex_buffer_binding_point,
          .format = GetVkFormat(type, static_cast<uint32_t>(count)),
          .offset = static_cast<uint32_t>(offset),
      };
      attribute_descriptions.push_back(description);
      offset += count * GetDataTypeSizeInBytes(type);
    }
    vertex_input_binding_descriptions.push_back({
        .binding = vertex_buffer_binding_point,
        .stride = stride,
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
    });
  }

  const VkPipelineVertexInputStateCreateInfo vertex_input_info = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
      .vertexBindingDescriptionCount = static_cast<uint32_t>(vertex_input_binding_descriptions.size()),
      .pVertexBindingDescriptions = vertex_input_binding_descriptions.data(),
      .vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_descriptions.size()),
      .pVertexAttributeDescriptions = attribute_descriptions.data(),
  };

  const VkGraphicsPipelineCreateInfo pipeline_info = {
      .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
      .stageCount = 2,
      .pStages = shader_stages.data(),
      .pVertexInputState = &vertex_input_info,
      .pInputAssemblyState = &input_assembly,
      .pTessellationState = VK_NULL_HANDLE,
      .pViewportState = &viewport_state,
      .pRasterizationState = &rasterizer,
      .pMultisampleState = &multisampling,
      .pDepthStencilState = &depth_stencil,
      .pColorBlendState = &color_blending,
      .pDynamicState = &dynamic_state_create_info,
      .layout = pipeline_layout_,
      .renderPass = render_pass_->GetRenderPass(),
      .subpass = 0,
      .basePipelineHandle = VK_NULL_HANDLE,
  };

  VK_CALL(vkCreateGraphicsPipelines(context_->GetDevice(), VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &pipeline_));

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

void vulkan::RenderingPipeline::SetVertexBuffer(const uint32_t &index, const std::shared_ptr<Buffer> &vertex_buffer) {
  if (vertex_buffer == nullptr) {
    vertex_buffers_.erase(index);
    return;
  }
  vertex_buffers_[index] = vertex_buffer;
}

void vulkan::RenderingPipeline::SetIndexBuffer(const std::shared_ptr<Buffer> &buffer, VkIndexType element_type) {
  this->index_buffer_ = buffer;
  this->index_type_ = element_type;
}
void vulkan::RenderingPipeline::SetImageView(uint32_t binding_point, const uint32_t descriptor_set_index,
                                             const std::shared_ptr<ImageView> &image_view,
                                             const std::shared_ptr<Sampler> &sampler) {
  if (!fragment_shader_->GetBindings().contains(binding_point) &&
      !vertex_shader_->GetBindings().contains(binding_point)) {
    spdlog::warn("warning: entry point {} does not exists in shaders", binding_point);
    return;
  }
  if (descriptor_sets_.size() <= descriptor_set_index) {
    throw std::runtime_error("invalid descriptor set index");
  }
  descriptor_sets_[descriptor_set_index].image_views[binding_point] = image_view;
  const VkDescriptorImageInfo image_info{
      .sampler = sampler->GetSampler(),
      .imageView = image_view->GetImageView(),
      .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
  };
  const VkWriteDescriptorSet descriptor_write{
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .pNext = nullptr,
      .dstSet = descriptor_sets_[descriptor_set_index].descriptor_set,
      .dstBinding = binding_point,
      .dstArrayElement = 0,
      .descriptorCount = 1,
      .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
      .pImageInfo = &image_info,
      .pBufferInfo = VK_NULL_HANDLE,
      .pTexelBufferView = VK_NULL_HANDLE,
  };
  vkUpdateDescriptorSets(context_->GetDevice(), 1, &descriptor_write, 0, nullptr);
}
void vulkan::RenderingPipeline::SetUniformBuffer(uint32_t binding_point, const uint32_t descriptor_set_index,
                                                 const std::shared_ptr<Buffer> &uniform_buffer) {
  auto fragment_binding = fragment_shader_->GetBindings();
  auto vertex_binding = vertex_shader_->GetBindings();
  if (!fragment_binding.contains(binding_point) && !vertex_binding.contains(binding_point)) {
    spdlog::warn("warning: entry point {} does not exists in shaders", binding_point);
    return;
  }
  const auto binding =
      fragment_binding.contains(binding_point) ? fragment_binding[binding_point] : vertex_binding[binding_point];

  if (auto expected_size = binding.block.size; expected_size != uniform_buffer->GetSizeInBytes()) {
    spdlog::warn("uniform buffer size mismatch expected:{} found{}", expected_size, uniform_buffer->GetSizeInBytes());
  }
  if (descriptor_sets_.size() <= descriptor_set_index) {
    throw std::runtime_error("invalid descriptor set index");
  }
  descriptor_sets_[descriptor_set_index].uniform_buffers[binding_point] = uniform_buffer;
  const VkDescriptorBufferInfo buffer_info{
      .buffer = uniform_buffer->GetBuffer(),
      .offset = 0,
      .range = uniform_buffer->GetSizeInBytes(),
  };
  const VkWriteDescriptorSet descriptor_write{
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .pNext = nullptr,
      .dstSet = descriptor_sets_[descriptor_set_index].descriptor_set,
      .dstBinding = binding_point,
      .dstArrayElement = 0,
      .descriptorCount = 1,
      .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
      .pImageInfo = VK_NULL_HANDLE,
      .pBufferInfo = &buffer_info,
      .pTexelBufferView = VK_NULL_HANDLE,
  };
  vkUpdateDescriptorSets(context_->GetDevice(), 1, &descriptor_write, 0, nullptr);
}

void vulkan::RenderingPipeline::DrawIndexed(const VkCommandBuffer &command_buffer, const uint32_t &index_count,
                                            const size_t &descriptor_index) const {
  vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_);
  const std::vector<VkDeviceSize> vertex_buffer_offset = {0};
  for (const auto &[index, buffer] : vertex_buffers_) {
    auto *vertex_buffer = buffer->GetBuffer();
    vkCmdBindVertexBuffers(command_buffer, index, 1, &vertex_buffer, vertex_buffer_offset.data());
  }
  vkCmdBindIndexBuffer(command_buffer, index_buffer_->GetBuffer(), 0, index_type_);
  vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_layout_, 0, 1,
                          &descriptor_sets_[descriptor_index].descriptor_set, 0, nullptr);
  vkCmdDrawIndexed(command_buffer, index_count, 1, 0, 0, 0);
}

void vulkan::RenderingPipeline::Draw(const VkCommandBuffer &command_buffer, const uint32_t &vertex_count,
                                     const size_t &descriptor_index) const {
  vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_);
  const std::vector<VkDeviceSize> vertex_buffer_offset = {0};
  for (const auto &[index, buffer] : vertex_buffers_) {
    auto *vertex_buffer = buffer->GetBuffer();
    vkCmdBindVertexBuffers(command_buffer, index, 1, &vertex_buffer, vertex_buffer_offset.data());
  }
  vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_layout_, 0, 1,
                          &descriptor_sets_[descriptor_index].descriptor_set, 0, nullptr);
  vkCmdDraw(command_buffer, vertex_count, 1, 0, 0);
}

size_t vulkan::RenderingPipeline::GetDescriptorSetCount() const { return descriptor_sets_.size(); }

vulkan::RenderingPipeline::~RenderingPipeline() {
  for (const auto &descriptor_set : descriptor_sets_) {
    vkFreeDescriptorSets(context_->GetDevice(), context_->GetDescriptorPool(), 1, &descriptor_set.descriptor_set);
  }
  vkDestroyDescriptorSetLayout(context_->GetDevice(), layout_, nullptr);
  vkDestroyPipelineLayout(context_->GetDevice(), pipeline_layout_, nullptr);
  vkDestroyPipeline(context_->GetDevice(), pipeline_, nullptr);
}

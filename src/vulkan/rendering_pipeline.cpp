#include "rendering_pipeline.hpp"

#include "utils.hpp"

#include <spdlog/spdlog.h>

#include <array>
#include <utility>

namespace vulkan {
VkFormat GetVkFormat(DataType type, uint32_t count) {
  switch (type) {
    case DataType::BYTE:
      switch (count) {
        case 1:return VK_FORMAT_R8_UNORM;
        case 2:return VK_FORMAT_R8G8_UNORM;
        case 3:return VK_FORMAT_R8G8B8_UNORM;
        case 4:return VK_FORMAT_R8G8B8A8_UNORM;
        default:throw std::runtime_error("unsupported count");
      }
    case DataType::UINT_16:
      switch (count) {
        case 1:return VK_FORMAT_R16_UINT;
        case 2:return VK_FORMAT_R16G16_UINT;
        case 3: return VK_FORMAT_R16G16B16_UINT;
        case 4: return VK_FORMAT_R16G16B16A16_UINT;
        default:throw std::runtime_error("unsupported count");
      }
    case DataType::UINT_32:
      switch (count) {
        case 1:return VK_FORMAT_R32_UINT;
        case 2:return VK_FORMAT_R32G32_UINT;
        case 3: return VK_FORMAT_R32G32B32_UINT;
        case 4: return VK_FORMAT_R32G32B32A32_UINT;
        default:throw std::runtime_error("unsupported count");
      }
    case DataType::FLOAT:
      switch (count) {
        case 1:return VK_FORMAT_R32_SFLOAT;
        case 2:return VK_FORMAT_R32G32_SFLOAT;
        case 3: return VK_FORMAT_R32G32B32_SFLOAT;
        case 4: return VK_FORMAT_R32G32B32A32_SFLOAT;
        default:throw std::runtime_error("unsupported count");
      }
    default:throw std::runtime_error("unsupported enum");
  }
}
}  // namespace vulkan

std::shared_ptr<vulkan::RenderingPipeline> vulkan::RenderingPipeline::Create(const std::shared_ptr<RenderingContext> &context,
                                                                             const std::shared_ptr<RenderPass> &render_pass,
                                                                             const std::shared_ptr<Shader> &vertex_shader,
                                                                             const std::shared_ptr<Shader> &fragment_shader,
                                                                             const vulkan::VertexBufferLayout &vbl,
                                                                             vulkan::RenderingPipelineConfig config,
                                                                             size_t descriptor_set_count) {
  auto pipeline =
      new RenderingPipeline(context, render_pass, vertex_shader, fragment_shader, vbl, config, descriptor_set_count);
  return std::shared_ptr<vulkan::RenderingPipeline>(pipeline);
}

vulkan::RenderingPipeline::RenderingPipeline(const std::shared_ptr<RenderingContext> &context,
                                             const std::shared_ptr<RenderPass> &render_pass,
                                             const std::shared_ptr<Shader> &vertex_shader,
                                             const std::shared_ptr<Shader> &fragment_shader,
                                             const VertexBufferLayout &vbl,
                                             RenderingPipelineConfig config,
                                             size_t descriptor_set_count)
    : context_(std::move(context)), render_pass_(render_pass),
      vertex_shader_(vertex_shader), fragment_shader_(fragment_shader) {
  std::array<VkPipelineShaderStageCreateInfo, 2> shader_stages = {vertex_shader_->GetShaderStageInfo(), fragment_shader_->GetShaderStageInfo()};
  auto vertex_bindings = vertex_shader_->GetBindings();
  auto fragment_bindings = fragment_shader_->GetBindings();
  auto pipeline_bindings = vertex_bindings;
  pipeline_bindings.reserve(pipeline_bindings.size() + fragment_bindings.size());
  pipeline_bindings.insert(pipeline_bindings.end(), fragment_bindings.begin(), fragment_bindings.end());

  VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info{
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
      .bindingCount = static_cast<uint32_t>(pipeline_bindings.size()),
      .pBindings = pipeline_bindings.data(),
  };

  VK_CALL(vkCreateDescriptorSetLayout(context_->GetDevice(), &descriptor_set_layout_create_info, nullptr, &layout_));

  VkPipelineInputAssemblyStateCreateInfo input_assembly = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
      .topology = config.draw_mode,
      .primitiveRestartEnable = VK_FALSE,
  };

  VkPipelineViewportStateCreateInfo viewport_state = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
      .viewportCount = 1,
      .pViewports = VK_NULL_HANDLE,
      .scissorCount = 1,
      .pScissors = VK_NULL_HANDLE,
  };

  VkPipelineRasterizationStateCreateInfo rasterizer = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
      .depthClampEnable = VK_FALSE,
      .rasterizerDiscardEnable = VK_FALSE,
      .polygonMode = VK_POLYGON_MODE_FILL,
      .cullMode = config.cull_mode,
      .frontFace = config.front_face,
      .depthBiasEnable = VK_FALSE,
      .lineWidth = 1.0f,
  };

  VkPipelineMultisampleStateCreateInfo multisampling = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
      .rasterizationSamples = config.sample_count,
      .sampleShadingEnable = VK_FALSE,
      .alphaToCoverageEnable = VK_FALSE,
  };

  VkPipelineColorBlendAttachmentState color_blend_attachment = {
      .blendEnable = VK_FALSE,
      .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT
          | VK_COLOR_COMPONENT_A_BIT,
  };

  VkPipelineColorBlendStateCreateInfo color_blending = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
      .logicOpEnable = VK_FALSE,
      .logicOp = VK_LOGIC_OP_COPY,
      .attachmentCount = 1,
      .pAttachments = &color_blend_attachment,
      .blendConstants = {0.0f, 0.0f, 0.0f, 0.0f},
  };

  VkPipelineDepthStencilStateCreateInfo depth_stencil = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
      .depthTestEnable = config.enable_depth_test,
      .depthWriteEnable = VK_TRUE,
      .depthCompareOp = config.depth_function,
      .depthBoundsTestEnable = VK_FALSE,
      .minDepthBounds = 0.0F,
      .maxDepthBounds = 1.0F,
  };

  VkPipelineLayoutCreateInfo pipeline_layout_info = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
      .setLayoutCount = 1,
      .pSetLayouts = &layout_,
  };

  VK_CALL(vkCreatePipelineLayout(context_->GetDevice(), &pipeline_layout_info, nullptr, &pipeline_layout_));

  std::vector<VkDynamicState>
      dynamic_states = {VkDynamicState::VK_DYNAMIC_STATE_VIEWPORT, VkDynamicState::VK_DYNAMIC_STATE_SCISSOR};
  VkPipelineDynamicStateCreateInfo dynamic_state_create_info{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
      .dynamicStateCount = static_cast<uint32_t>(dynamic_states.size()),
      .pDynamicStates = dynamic_states.data(),
  };

  const auto &kElements = vbl.GetElements();
  auto stride = static_cast<uint32_t>(vbl.GetElementSize());
  size_t offset = 0;
  std::vector<VkVertexInputAttributeDescription> attribute_descriptions{};
  for (auto element: kElements) {
    VkVertexInputAttributeDescription description{
        .location = element.binding_index,
        .binding = 0,
        .format = GetVkFormat(element.type, static_cast<uint32_t>(element.count)),
        .offset = static_cast<uint32_t>(offset),
    };
    attribute_descriptions.push_back(description);
    offset += element.count * GetDataTypeSizeInBytes(element.type);
  }

  VkVertexInputBindingDescription vertex_input_binding_description{
      .binding = 0,
      .stride = stride,
      .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
  };

  VkPipelineVertexInputStateCreateInfo vertex_input_info = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
      .vertexBindingDescriptionCount = 1,
      .pVertexBindingDescriptions = &vertex_input_binding_description,
      .vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_descriptions.size()),
      .pVertexAttributeDescriptions = attribute_descriptions.data(),
  };

  VkGraphicsPipelineCreateInfo pipeline_info = {
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
  for (auto i = 0; i < descriptor_set_count; i++) {
    VkDescriptorSetAllocateInfo alloc_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = context_->GetDescriptorPool(),
        .descriptorSetCount = 1,
        .pSetLayouts = &layout_,
    };
    VK_CALL(vkAllocateDescriptorSets(context_->GetDevice(), &alloc_info, &descriptor_sets_[i].descriptor_set));
  }
}

void vulkan::RenderingPipeline::SetVertexBuffer(const std::shared_ptr<Buffer> &vertex_buffer) {
  vertex_buffer_ = vertex_buffer;
}

void vulkan::RenderingPipeline::SetIndexBuffer(const std::shared_ptr<Buffer> &buffer,
                                               VkIndexType element_type) {
  this->index_buffer_ = buffer;
  this->index_type_ = element_type;
}
void vulkan::RenderingPipeline::SetImageView(unsigned int binding_point,
                                             unsigned int descriptor_set_index,
                                             const std::shared_ptr<ImageView> &image_view,
                                             const std::shared_ptr<Sampler> &sampler) {
  auto check_shader = [&binding_point](std::shared_ptr<Shader> shader) -> bool {
    auto shader_bindings = shader->GetBindings();
    return std::any_of(std::begin(shader_bindings), std::end(shader_bindings), [&binding_point](auto binding) {
      return binding.binding == binding_point && binding.descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    });
  };
  if (!check_shader(fragment_shader_) && !check_shader(vertex_shader_)) {
    spdlog::warn("warning: entry point {} does not exists in shaders", binding_point);
    return;
  }
  if (descriptor_sets_.size() <= descriptor_set_index) {
    throw std::runtime_error("invalid descriptor set index");
  }
  descriptor_sets_[descriptor_set_index].image_views[binding_point] = image_view;
  VkDescriptorImageInfo image_info{
      .sampler = sampler->GetSampler(),
      .imageView = image_view->GetImageView(),
      .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
  };
  VkWriteDescriptorSet descriptor_write{
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
  vkUpdateDescriptorSets(context_->GetDevice(),
                         1,
                         &descriptor_write,
                         0,
                         nullptr);
}
void vulkan::RenderingPipeline::SetUniformBuffer(unsigned int binding_point,
                                                 unsigned int descriptor_set_index,
                                                 const std::shared_ptr<Buffer> &uniform_buffer) {
  auto check_shader = [&binding_point, &uniform_buffer](const std::shared_ptr<Shader> &shader) -> bool {
    for (auto binding: shader->GetBindings()) {
      if (binding.binding == binding_point && binding.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
        auto expected_size = shader->DescriptorSizeInBytes(binding_point);
        if (expected_size != uniform_buffer->GetSizeInBytes()) {
          spdlog::warn("uniform buffer size mismatch expected:{} found{}",
                       expected_size,
                       uniform_buffer->GetSizeInBytes());
        }
        return true;
      }
    }
    return false;
  };
  if (!check_shader(fragment_shader_) && !check_shader(vertex_shader_)) {
    spdlog::warn("warning: entry point {} does not exists in shaders", binding_point);
    return;
  }
  if (descriptor_sets_.size() <= descriptor_set_index) {
    throw std::runtime_error("invalid descriptor set index");
  }
  descriptor_sets_[descriptor_set_index].uniform_buffers[binding_point] = uniform_buffer;
  VkDescriptorBufferInfo buffer_info{
      .buffer = uniform_buffer->GetBuffer(),
      .offset = 0,
      .range = uniform_buffer->GetSizeInBytes(),
  };
  VkWriteDescriptorSet descriptor_write{
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
  vkUpdateDescriptorSets(context_->GetDevice(),
                         1,
                         &descriptor_write,
                         0,
                         nullptr);
}

void vulkan::RenderingPipeline::Draw(VkCommandBuffer command_buffer,
                                     size_t index_count,
                                     size_t offset,
                                     size_t descriptor_index) {
  vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_);
  std::vector<VkDeviceSize> offsets = {0};
  auto *buffer = vertex_buffer_->GetBuffer();
  vkCmdBindVertexBuffers(command_buffer, 0, 1, &buffer, offsets.data());
  vkCmdBindIndexBuffer(command_buffer, index_buffer_->GetBuffer(), offset, index_type_);
  vkCmdBindDescriptorSets(command_buffer,
                          VK_PIPELINE_BIND_POINT_GRAPHICS,
                          pipeline_layout_,
                          0,
                          1,
                          &descriptor_sets_[descriptor_index].descriptor_set,
                          0,
                          nullptr);
  vkCmdDrawIndexed(command_buffer, static_cast<uint32_t>(index_count), 1, 0, 0, 0);
}

size_t vulkan::RenderingPipeline::GetDescriptorSetCount() {
  return descriptor_sets_.size();
}

vulkan::RenderingPipeline::~RenderingPipeline() {
  context_->WaitForGpuIdle();
  for (const auto &kDescriptorSet: descriptor_sets_) {
    vkFreeDescriptorSets(context_->GetDevice(),
                         context_->GetDescriptorPool(),
                         1,
                         &kDescriptorSet.descriptor_set);
  }
  vkDestroyDescriptorSetLayout(context_->GetDevice(), layout_, nullptr);
  vkDestroyPipelineLayout(context_->GetDevice(), pipeline_layout_, nullptr);
  vkDestroyPipeline(context_->GetDevice(), pipeline_, nullptr);
}

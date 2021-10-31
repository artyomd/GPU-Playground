//
// Created by artyomd on 3/4/20.
//

#include "src/api/vulkan/vulkan_rendering_pipeline.hpp"

#include <utility>

#include "src/api/vulkan/vulkan_shader.hpp"

api::vulkan::VulkanRenderingPipeline::VulkanRenderingPipeline(std::shared_ptr<VulkanRenderingContext> context,
                                                              std::shared_ptr<Shader> vertex_shader,
                                                              std::shared_ptr<Shader> fragment_shader,
                                                              const VertexBufferLayout &vbl,
                                                              RenderingPipelineConfig config) :
    RenderingPipeline(),
    context_(std::move(context)),
    device_(context_->GetDevice()),
    config_(config) {
  this->vertex_shader_ = std::dynamic_pointer_cast<VulkanShader>(vertex_shader);
  this->fragment_shader_ = std::dynamic_pointer_cast<VulkanShader>(fragment_shader);
  viewport_.x = 0.0F;
  viewport_.y = 0.0F;
  viewport_.width = 0.0F;
  viewport_.height = 0.0F;
  viewport_.minDepth = 0.0F;
  viewport_.maxDepth = 1.0F;
  scissor_.offset = {0, 0};
  scissor_.extent = {0, 0};
  CreatePipeline(vbl);
}

void api::vulkan::VulkanRenderingPipeline::SetVertexBuffer(std::shared_ptr<Buffer> buffer) {
  this->vertex_buffer_ = std::dynamic_pointer_cast<VulkanBuffer>(buffer);
}

void api::vulkan::VulkanRenderingPipeline::SetIndexBuffer(std::shared_ptr<Buffer> buffer, api::DataType element_type) {
  this->index_buffer_ = std::dynamic_pointer_cast<VulkanBuffer>(buffer);
  this->index_type_ = GetVkType(element_type);
}

void api::vulkan::VulkanRenderingPipeline::CreatePipeline(const VertexBufferLayout &vbl) {
  VkPipelineShaderStageCreateInfo shader_stages[] = {
      vertex_shader_->GetShaderStageInfo(),
      fragment_shader_->GetShaderStageInfo()
  };
  auto vertex_bindings = vertex_shader_->GetBindings();
  auto fragment_bindings = fragment_shader_->GetBindings();
  auto pipeline_bindings = vertex_bindings;
  pipeline_bindings.reserve(pipeline_bindings.size() + fragment_bindings.size());
  pipeline_bindings.insert(pipeline_bindings.end(), fragment_bindings.begin(), fragment_bindings.end());

  VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info{};
  descriptor_set_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  descriptor_set_layout_create_info.bindingCount = static_cast<uint32_t>(pipeline_bindings.size());
  descriptor_set_layout_create_info.pBindings = pipeline_bindings.data();
  if (vkCreateDescriptorSetLayout(device_, &descriptor_set_layout_create_info, nullptr, &layout_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create shader module!");
  }

  VkPipelineInputAssemblyStateCreateInfo input_assembly = {};
  input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  input_assembly.topology = GetVkDrawMode(config_.draw_mode);
  input_assembly.primitiveRestartEnable = VK_FALSE;

  VkPipelineViewportStateCreateInfo viewport_state = {};
  viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewport_state.viewportCount = 1;
  viewport_state.pViewports = VK_NULL_HANDLE;
  viewport_state.scissorCount = 1;
  viewport_state.pScissors = VK_NULL_HANDLE;

  VkPipelineRasterizationStateCreateInfo rasterizer = {};
  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer.lineWidth = 1.0F;
  rasterizer.cullMode = GetVkCullMode(config_.cull_mode);
  rasterizer.frontFace = GetVkFrontFace(config_.front_face);
  rasterizer.depthBiasEnable = VK_FALSE;

  VkPipelineMultisampleStateCreateInfo multisampling = {};
  multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.alphaToCoverageEnable = VK_FALSE;
  multisampling.rasterizationSamples = context_->GetRecommendedMsaaSamples();

  VkPipelineColorBlendAttachmentState color_blend_attachment = {};
  color_blend_attachment.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  color_blend_attachment.blendEnable = VK_FALSE;

  VkPipelineColorBlendStateCreateInfo color_blending = {};
  color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  color_blending.logicOpEnable = VK_FALSE;
  color_blending.logicOp = VK_LOGIC_OP_COPY;
  color_blending.attachmentCount = 1;
  color_blending.pAttachments = &color_blend_attachment;
  color_blending.blendConstants[0] = 0.0F;
  color_blending.blendConstants[1] = 0.0F;
  color_blending.blendConstants[2] = 0.0F;
  color_blending.blendConstants[3] = 0.0F;

  VkPipelineDepthStencilStateCreateInfo depth_stencil = {};
  depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depth_stencil.depthTestEnable = config_.enable_depth_test;
  depth_stencil.depthWriteEnable = VK_TRUE;
  depth_stencil.depthCompareOp = GetVkCompareOp(config_.depth_function);
  depth_stencil.depthBoundsTestEnable = VK_FALSE;
  depth_stencil.minDepthBounds = 0.0F;
  depth_stencil.maxDepthBounds = 1.0F;

  VkPipelineLayoutCreateInfo pipeline_layout_info = {};
  pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipeline_layout_info.setLayoutCount = 1;
  pipeline_layout_info.pSetLayouts = &layout_;
  if (vkCreatePipelineLayout(device_, &pipeline_layout_info, nullptr, &pipeline_layout_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create pipeline layout!");
  }

  VkPipelineDynamicStateCreateInfo dynamic_state_create_info{};
  dynamic_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  std::vector<VkDynamicState> dynamic_states = {VkDynamicState::VK_DYNAMIC_STATE_VIEWPORT,
                                                VkDynamicState::VK_DYNAMIC_STATE_SCISSOR};
  dynamic_state_create_info.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
  dynamic_state_create_info.pDynamicStates = dynamic_states.data();

  const auto &elements = vbl.GetElements();
  auto stride = static_cast<uint32_t>(vbl.GetElementSize());
  size_t offset = 0;
  std::vector<VkVertexInputAttributeDescription> attribute_descriptions{};
  for (auto element: elements) {
    VkVertexInputAttributeDescription description{};
    description.binding = 0;
    description.location = element.binding_index;
    description.format = GetVkFormat(element.type, static_cast<uint32_t>(element.count));
    description.offset = static_cast<uint32_t>(offset);
    attribute_descriptions.push_back(description);
    offset += element.count * GetDataTypeSizeInBytes(element.type);
  }

  VkVertexInputBindingDescription vertex_input_binding_description{};
  vertex_input_binding_description.binding = 0;
  vertex_input_binding_description.stride = stride;
  vertex_input_binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  VkPipelineVertexInputStateCreateInfo vertex_input_info = {};
  vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertex_input_info.vertexBindingDescriptionCount = 1;
  vertex_input_info.pVertexBindingDescriptions = &vertex_input_binding_description;
  vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_descriptions.size());
  vertex_input_info.pVertexAttributeDescriptions = attribute_descriptions.data();

  VkGraphicsPipelineCreateInfo pipeline_info = {};
  pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipeline_info.stageCount = 2;
  pipeline_info.pStages = shader_stages;
  pipeline_info.pVertexInputState = &vertex_input_info;
  pipeline_info.pInputAssemblyState = &input_assembly;
  pipeline_info.pTessellationState = VK_NULL_HANDLE;
  pipeline_info.pViewportState = &viewport_state;
  pipeline_info.pRasterizationState = &rasterizer;
  pipeline_info.pMultisampleState = &multisampling;
  pipeline_info.pDepthStencilState = &depth_stencil;
  pipeline_info.pColorBlendState = &color_blending;
  pipeline_info.pDynamicState = VK_NULL_HANDLE;
  pipeline_info.layout = pipeline_layout_;
  pipeline_info.renderPass = context_->GetRenderPass();
  pipeline_info.subpass = 0;
  pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
  pipeline_info.pDynamicState = &dynamic_state_create_info;

  if (vkCreateGraphicsPipelines(device_, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &pipeline_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create graphics pipeline!");
  }

  auto image_count = context_->GetImageCount();
  std::vector<VkDescriptorSetLayout> layouts(image_count, layout_);
  VkDescriptorSetAllocateInfo alloc_info = {};
  alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  alloc_info.descriptorPool = context_->GetDescriptorPool();
  alloc_info.descriptorSetCount = static_cast<uint32_t>(image_count);
  alloc_info.pSetLayouts = layouts.data();
  descriptor_sets_.resize(image_count);
  if (vkAllocateDescriptorSets(device_, &alloc_info, descriptor_sets_.data()) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate descriptor sets!");
  }
  CreateUniformBuffers(vertex_shader_);
  CreateUniformBuffers(fragment_shader_);
}

void api::vulkan::VulkanRenderingPipeline::CreateUniformBuffers(const std::shared_ptr<VulkanShader> &shader) {
  std::vector<VkWriteDescriptorSet> descriptor_writes{};
  for (auto binding: shader->GetBindings()) {
    if (binding.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
        && uniform_buffers_.find(binding.binding) == uniform_buffers_.end()) {
      auto size = shader->DescriptorSizeInBytes(binding.binding);
      std::vector<std::shared_ptr<VulkanBuffer>> buffers;
      buffers.reserve(context_->GetImageCount());
      for (uint32_t i = 0; i < context_->GetImageCount(); i++) {
        auto vulkan_buffer = std::make_shared<VulkanBuffer>(context_,
                                                            size,
                                                            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                                            GetVkMemoryType(MemoryType::HOST_VISIBLE));
        buffers.emplace_back(vulkan_buffer);

        VkDescriptorBufferInfo descriptor_buffer_info{};
        descriptor_buffer_info.buffer = vulkan_buffer->GetBuffer();
        descriptor_buffer_info.offset = 0;
        descriptor_buffer_info.range = vulkan_buffer->GetSizeInBytes();

        VkWriteDescriptorSet write_descriptor_set{};
        write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write_descriptor_set.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        write_descriptor_set.descriptorCount = 1;
        write_descriptor_set.pBufferInfo = &descriptor_buffer_info;
        write_descriptor_set.dstBinding = binding.binding;
        write_descriptor_set.dstArrayElement = 0;
        write_descriptor_set.dstSet = descriptor_sets_[i];
        descriptor_writes.push_back(write_descriptor_set);
      }
      uniform_buffers_[binding.binding] = buffers;
    }
  }
  vkUpdateDescriptorSets(device_,
                         static_cast<uint32_t>(descriptor_writes.size()),
                         descriptor_writes.data(),
                         0,
                         nullptr);
}

void api::vulkan::VulkanRenderingPipeline::DestroyPipeline() {
  vkDestroyPipeline(device_, pipeline_, nullptr);
}

void api::vulkan::VulkanRenderingPipeline::Draw(size_t index_count, size_t offset) {
  auto command_buffer = context_->GetCurrentCommandBuffer();
  vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_);
  vkCmdSetViewport(command_buffer, 0, 1, &this->viewport_);
  vkCmdSetScissor(command_buffer, 0, 1, &this->scissor_);
  VkDeviceSize offsets[] = {0};
  auto buffer = vertex_buffer_->GetBuffer();
  vkCmdBindVertexBuffers(command_buffer, 0, 1, &buffer, offsets);
  vkCmdBindIndexBuffer(command_buffer, index_buffer_->GetBuffer(), offset, this->index_type_);
  vkCmdBindDescriptorSets(command_buffer,
                          VK_PIPELINE_BIND_POINT_GRAPHICS,
                          pipeline_layout_,
                          0,
                          1,
                          &descriptor_sets_[context_->GetCurrentImageIndex()],
                          0,
                          nullptr);
  vkCmdDrawIndexed(context_->GetCurrentCommandBuffer(), static_cast<uint32_t>(index_count), 1, 0, 0, 0);
}

void api::vulkan::VulkanRenderingPipeline::UpdateUniformBuffer(unsigned int binding_point, void *data) {
  if (uniform_buffers_.find(binding_point) == uniform_buffers_.end()) {
    throw std::runtime_error("invalid binding point");
  }
  for (const auto &buffer: uniform_buffers_[binding_point]) {
    buffer->Update(data);
  }
}

void api::vulkan::VulkanRenderingPipeline::SetTexture(unsigned int binding_point,
                                                      std::shared_ptr<api::Texture2D> texture) {
  bool binding_available = false;
  for (auto binding: fragment_shader_->GetBindings()) {
    if (binding.binding == binding_point && binding.descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) {
      binding_available = true;
    }
  }
  if (!binding_available) {
    for (auto binding: vertex_shader_->GetBindings()) {
      if (binding.binding == binding_point && binding.descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) {
        binding_available = true;
      }
    }
  }
  if (!binding_available) {
    std::cout << "warning: entry point " << binding_point << " does not exists in shaders" << std::endl;
    return;
  }
  auto vk_uniform = std::dynamic_pointer_cast<VulkanTexture2D>(texture);
  this->textures_[binding_point] = vk_uniform;
  std::vector<VkWriteDescriptorSet> descriptor_writes{};
  for (uint32_t i = 0; i < context_->GetImageCount(); i++) {
    auto descriptor_write = vk_uniform->GetWriteDescriptorSetFor(i, binding_point);
    descriptor_write.dstSet = descriptor_sets_[i];
    descriptor_writes.push_back(descriptor_write);
  }
  vkUpdateDescriptorSets(device_,
                         static_cast<uint32_t>(descriptor_writes.size()),
                         descriptor_writes.data(),
                         0,
                         nullptr);
}

void api::vulkan::VulkanRenderingPipeline::SetViewPort(uint32_t width, uint32_t height) {
  viewport_.width = static_cast<float>(width);
  viewport_.height = -static_cast<float>(height);
  viewport_.x = 0.0F;
  viewport_.y = static_cast<float>(height);
  scissor_.extent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
}

api::vulkan::VulkanRenderingPipeline::~VulkanRenderingPipeline() {
  context_->WaitForGpuIdle();
  vkFreeDescriptorSets(device_,
                       context_->GetDescriptorPool(),
                       static_cast<uint32_t>(descriptor_sets_.size()),
                       descriptor_sets_.data());
  vkDestroyDescriptorSetLayout(device_, layout_, nullptr);
  vkDestroyPipelineLayout(device_, pipeline_layout_, nullptr);
  DestroyPipeline();
}

//
// Created by artyomd on 3/4/20.
//

#include "src/api/vulkan/vulkan_rendering_pipeline.hpp"

#include <utility>

#include "src/api/vulkan/vulkan_shader.hpp"
#include "src/api/vulkan/vulkan_vertex_binding.hpp"

api::vulkan::VulkanRenderingPipeline::VulkanRenderingPipeline(std::shared_ptr<VulkanRenderingContext> context,
                                                              std::shared_ptr<VertexBinding> vertex_binding,
                                                              std::shared_ptr<IndexBuffer> index_buffer,
                                                              std::shared_ptr<Shader> vertex_shader,
                                                              std::shared_ptr<Shader> fragment_shader,
                                                              std::shared_ptr<RenderingPipelineLayout> pipeline_layout,
                                                              api::RenderingPipelineLayoutConfig config) :
    RenderingPipeline(
        std::move(vertex_binding),
        std::move(index_buffer),
        std::move(vertex_shader),
        std::move(fragment_shader),
        std::move(pipeline_layout),
        config),
    context_(std::move(context)),
    device_(context_->GetDevice()) {
  CreatePipeline();
}

void api::vulkan::VulkanRenderingPipeline::Render() {
  vkCmdBindPipeline(context_->GetCurrentCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_);
  vertex_binding_->Bind();
  index_buffer_->Bind();
  pipeline_layout_->Bind();
  vkCmdDrawIndexed(context_->GetCurrentCommandBuffer(), index_buffer_->GetCount(), 1, 0, 0, 0);
}

void api::vulkan::VulkanRenderingPipeline::ViewportChanged() {
  context_->WaitForGpuIdle();
  DestroyPipeline();
  CreatePipeline();
}

void api::vulkan::VulkanRenderingPipeline::CreatePipeline() {
  VkPipelineShaderStageCreateInfo shader_stages[] = {
      std::dynamic_pointer_cast<VulkanShader>(vertex_shader_)->GetShaderStageInfo(),
      std::dynamic_pointer_cast<VulkanShader>(fragment_shader_)->GetShaderStageInfo()
  };

  auto attribute_descriptions =
      std::dynamic_pointer_cast<VulkanVertexBinding>(vertex_binding_)->GetAttributeDescriptions();
  auto input_binding_description =
      std::dynamic_pointer_cast<VulkanVertexBinding>(vertex_binding_)->GetVertexInputBindingDescription();

  VkPipelineVertexInputStateCreateInfo vertex_input_info = {};
  vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertex_input_info.vertexBindingDescriptionCount = 1;
  vertex_input_info.pVertexBindingDescriptions = input_binding_description;
  vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_descriptions->size());
  vertex_input_info.pVertexAttributeDescriptions = attribute_descriptions->data();

  VkPipelineInputAssemblyStateCreateInfo input_assembly = {};
  input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  input_assembly.primitiveRestartEnable = VK_FALSE;

  auto swap_chain_extent = context_->GetSwapChainExtent();
  VkViewport viewport = {};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = (float) swap_chain_extent.width;
  viewport.height = (float) swap_chain_extent.height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  VkRect2D scissor = {};
  scissor.offset = {0, 0};
  scissor.extent = swap_chain_extent;

  VkPipelineViewportStateCreateInfo viewport_state = {};
  viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewport_state.viewportCount = 1;
  viewport_state.pViewports = &viewport;
  viewport_state.scissorCount = 1;
  viewport_state.pScissors = &scissor;

  VkCullModeFlags cull_mode;
  VkFrontFace front_face;
  switch (config_.cull_mode) {
    case CullMode::BACK: cull_mode = VK_CULL_MODE_BACK_BIT;
      break;
    case CullMode::FRONT: cull_mode = VK_CULL_MODE_FRONT_BIT;
      break;
    case CullMode::FRONT_AND_BACK: cull_mode = VK_CULL_MODE_FRONT_AND_BACK;
      break;
    case CullMode::NONE: cull_mode = VK_CULL_MODE_NONE;
      break;
    default: throw std::runtime_error("not implemented");
  }
  switch (config_.front_face) {
    case FrontFace::CW: front_face = VK_FRONT_FACE_CLOCKWISE;
      break;
    case FrontFace::CCW: front_face = VK_FRONT_FACE_COUNTER_CLOCKWISE;
      break;
    default: throw std::runtime_error("not implemented");
  }

  VkPipelineRasterizationStateCreateInfo rasterizer = {};
  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer.lineWidth = 1.0f;
  rasterizer.cullMode = cull_mode;
  rasterizer.frontFace = front_face;
  rasterizer.depthBiasEnable = VK_FALSE;

  VkPipelineMultisampleStateCreateInfo multisampling = {};
  multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.rasterizationSamples = context_->GetMsaaSamples();

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
  color_blending.blendConstants[0] = 0.0f;
  color_blending.blendConstants[1] = 0.0f;
  color_blending.blendConstants[2] = 0.0f;
  color_blending.blendConstants[3] = 0.0f;

  VkCompareOp compare_op;
  switch (config_.depth_function) {
    case DepthFunction::LESS: compare_op = VK_COMPARE_OP_LESS;
      break;
    default: throw std::runtime_error("not implemented");
  }
  VkPipelineDepthStencilStateCreateInfo depth_stencil = {};
  depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depth_stencil.depthTestEnable = config_.enable_depth_test;
  depth_stencil.depthWriteEnable = VK_TRUE;
  depth_stencil.depthCompareOp = compare_op;
  depth_stencil.depthBoundsTestEnable = VK_FALSE;
  depth_stencil.minDepthBounds = 0.0f;
  depth_stencil.maxDepthBounds = 1.0f;

  VkGraphicsPipelineCreateInfo pipeline_info = {};
  pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipeline_info.stageCount = 2;
  pipeline_info.pDepthStencilState = &depth_stencil;
  pipeline_info.pStages = shader_stages;
  pipeline_info.pVertexInputState = &vertex_input_info;
  pipeline_info.pInputAssemblyState = &input_assembly;
  pipeline_info.pViewportState = &viewport_state;
  pipeline_info.pRasterizationState = &rasterizer;
  pipeline_info.pMultisampleState = &multisampling;
  pipeline_info.pColorBlendState = &color_blending;
  pipeline_info.layout =
      std::dynamic_pointer_cast<VulkanRenderingPipelineLayout>(pipeline_layout_)->GetPipelineLayout();
  pipeline_info.renderPass = context_->GetVkRenderPass();
  pipeline_info.subpass = 0;
  pipeline_info.basePipelineHandle = VK_NULL_HANDLE;

  if (vkCreateGraphicsPipelines(device_, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &pipeline_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create graphics pipeline!");
  }
}

void api::vulkan::VulkanRenderingPipeline::DestroyPipeline() {
  vkDestroyPipeline(device_, pipeline_, nullptr);
}

api::vulkan::VulkanRenderingPipeline::~VulkanRenderingPipeline() {
  DestroyPipeline();
}

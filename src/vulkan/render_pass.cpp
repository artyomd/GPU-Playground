#include "render_pass.hpp"

#include "utils.hpp"

vulkan::RenderPass::RenderPass(const std::shared_ptr<RenderingContext>& rendering_context,
                               const VkFormat color_attachment_format, const VkFormat depth_attachment_format,
                               const VkSampleCountFlagBits color_attachment_sample_count)
    : rendering_context_(rendering_context),
      color_attachment_format_(color_attachment_format),
      depth_attachment_format_(depth_attachment_format),
      sample_count_(color_attachment_sample_count) {
  const VkAttachmentDescription2 color_attachment = {
      .sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2,
      .pNext = nullptr,
      .flags = 0,
      .format = color_attachment_format_,
      .samples = sample_count_,
      .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
      .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
      .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
      .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
      .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
      .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
  };

  const VkAttachmentDescription2 depth_attachment = {
      .sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2,
      .pNext = nullptr,
      .flags = 0,
      .format = depth_attachment_format_,
      .samples = sample_count_,
      .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
      .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
      .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
      .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
      .initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
      .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
  };

  const VkAttachmentDescription2 color_attachment_resolve = {
      .sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2,
      .pNext = nullptr,
      .flags = 0,
      .format = color_attachment_format_,
      .samples = VK_SAMPLE_COUNT_1_BIT,
      .loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
      .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
      .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
      .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
      .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
      .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
  };

  std::vector attachments = {color_attachment};
  if (depth_attachment_format_ != VK_FORMAT_UNDEFINED) {
    attachments.push_back(depth_attachment);
  }
  if (sample_count_ != VK_SAMPLE_COUNT_1_BIT) {
    attachments.push_back(color_attachment_resolve);
  }

  constexpr VkAttachmentReference2 color_attachment_ref = {
      .sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2,
      .pNext = nullptr,
      .attachment = 0,
      .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
      .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
  };

  constexpr VkAttachmentReference2 depth_attachment_ref = {
      .sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2,
      .pNext = nullptr,
      .attachment = 1,
      .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
      .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
  };

  const VkAttachmentReference2 color_attachment_resolve_ref = {
      .sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2,
      .pNext = nullptr,
      .attachment = (depth_attachment_format == VK_FORMAT_UNDEFINED) ? 1u : 2u,
      .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
      .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
  };

  const VkSubpassDescription2 sub_pass = {
      .sType = VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2,
      .pNext = nullptr,
      .flags = 0,
      .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
      .viewMask = 0,
      .inputAttachmentCount = 0,
      .pInputAttachments = nullptr,
      .colorAttachmentCount = 1,
      .pColorAttachments = &color_attachment_ref,
      .pResolveAttachments = sample_count_ == VK_SAMPLE_COUNT_1_BIT ? nullptr : &color_attachment_resolve_ref,
      .pDepthStencilAttachment = depth_attachment_format == VK_FORMAT_UNDEFINED ? nullptr : &depth_attachment_ref,
      .preserveAttachmentCount = 0,
      .pPreserveAttachments = nullptr,
  };

  constexpr VkSubpassDependency2 dependency = {
      .sType = VK_STRUCTURE_TYPE_SUBPASS_DEPENDENCY_2,
      .pNext = nullptr,
      .srcSubpass = VK_SUBPASS_EXTERNAL,
      .dstSubpass = 0,
      .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
      .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
      .srcAccessMask = 0,
      .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
      .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
      .viewOffset = 0,
  };

  const VkRenderPassCreateInfo2 render_pass_info = {
      .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2,
      .pNext = nullptr,
      .flags = 0,
      .attachmentCount = static_cast<uint32_t>(attachments.size()),
      .pAttachments = attachments.data(),
      .subpassCount = 1,
      .pSubpasses = &sub_pass,
      .dependencyCount = 1,
      .pDependencies = &dependency,
      .correlatedViewMaskCount = 0,
      .pCorrelatedViewMasks = nullptr,
  };

  VK_CALL(vkCreateRenderPass2(rendering_context_->GetDevice(), &render_pass_info, nullptr, &render_pass_));
}

VkFormat vulkan::RenderPass::GetColorAttachmentFormat() const { return color_attachment_format_; }

VkRenderPass vulkan::RenderPass::GetRenderPass() const { return render_pass_; }

vulkan::RenderPass::~RenderPass() { vkDestroyRenderPass(rendering_context_->GetDevice(), render_pass_, nullptr); }

VkFormat vulkan::RenderPass::GetDepthAttachmentFormat() const { return depth_attachment_format_; }

VkSampleCountFlagBits vulkan::RenderPass::GetSampleCount() const { return sample_count_; }


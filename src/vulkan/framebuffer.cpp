#include "framebuffer.hpp"

#include "utils.hpp"

std::shared_ptr<vulkan::Framebuffer> vulkan::Framebuffer::Create(
    const std::shared_ptr<RenderingContext>& rendering_context, const std::shared_ptr<RenderPass>& render_pass,
    const std::vector<std::shared_ptr<ImageView>>& attachments) {
  return std::shared_ptr<Framebuffer>(new Framebuffer(rendering_context, render_pass, attachments));
}

vulkan::Framebuffer::Framebuffer(const std::shared_ptr<RenderingContext>& rendering_context,
                                 const std::shared_ptr<RenderPass>& render_pass,
                                 const std::vector<std::shared_ptr<ImageView>>& attachments)
    : rendering_context_(rendering_context), render_pass_(render_pass), attachments_(attachments) {
  auto vk_attachments = std::vector<VkImageView>(attachments_.size());
  for (auto i = 0; i < attachments_.size(); i++) {
    vk_attachments[i] = attachments_[i]->GetImageView();
  }
  const VkFramebufferCreateInfo framebuffer_create_info{
      .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .renderPass = render_pass_->GetRenderPass(),
      .attachmentCount = static_cast<uint32_t>(vk_attachments.size()),
      .pAttachments = vk_attachments.data(),
      .width = attachments_[0]->GetWidth(),
      .height = attachments_[0]->GetHeight(),
      .layers = 1,
  };

  VK_CALL(vkCreateFramebuffer(rendering_context_->GetDevice(), &framebuffer_create_info, nullptr, &framebuffer_));
}

VkFramebuffer vulkan::Framebuffer::GetFramebuffer() const { return framebuffer_; }
uint32_t vulkan::Framebuffer::GetWidth() const { return attachments_[0]->GetWidth(); }
uint32_t vulkan::Framebuffer::GetHeight() const { return attachments_[0]->GetHeight(); }

vulkan::Framebuffer::~Framebuffer() { vkDestroyFramebuffer(rendering_context_->GetDevice(), framebuffer_, nullptr); }

std::shared_ptr<vulkan::ImageView> vulkan::Framebuffer::GetDepthAttachment() const {
  for (auto attachment : attachments_) {
    if (attachment->GetAspectMask() == VK_IMAGE_ASPECT_DEPTH_BIT) {
      return attachment;
    }
  }
  return nullptr;
}

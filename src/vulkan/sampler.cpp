#include "sampler.hpp"

#include "utils.hpp"

std::shared_ptr<vulkan::Sampler> vulkan::Sampler::Create(const std::shared_ptr<RenderingContext> &context,
                                                         const VkFilter mag_filter, const VkFilter min_filter,
                                                         const VkSamplerAddressMode address_mode_u,
                                                         const VkSamplerAddressMode address_mode_v) {
  return std::shared_ptr<Sampler>(new Sampler(context, mag_filter, min_filter, address_mode_u, address_mode_v));
}

vulkan::Sampler::Sampler(const std::shared_ptr<RenderingContext> &context, const VkFilter mag_filter,
                         const VkFilter min_filter, const VkSamplerAddressMode address_mode_u,
                         const VkSamplerAddressMode address_mode_v)
    : context_(context) {
  const VkSamplerCreateInfo sampler_info = {
      .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
      .magFilter = mag_filter,
      .minFilter = min_filter,
      .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
      .addressModeU = address_mode_u,
      .addressModeV = address_mode_v,
      .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
      .anisotropyEnable = VK_FALSE,
      .maxAnisotropy = 1.0F,
      .compareEnable = VK_FALSE,
      .compareOp = VK_COMPARE_OP_ALWAYS,
      .borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
      .unnormalizedCoordinates = VK_FALSE,
  };

  VK_CALL(vkCreateSampler(context->GetDevice(), &sampler_info, nullptr, &sampler_));
}
vulkan::Sampler::~Sampler() { vkDestroySampler(context_->GetDevice(), sampler_, nullptr); }
VkSampler vulkan::Sampler::GetSampler() const { return sampler_; }

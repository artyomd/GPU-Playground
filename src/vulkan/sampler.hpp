#pragma once

#include "rendering_context.hpp"

#include <cstddef>

namespace vulkan {
class Sampler {
 public:
  static std::shared_ptr<vulkan::Sampler> Create(const std::shared_ptr<RenderingContext> &context,
                                                 VkFilter mag_filter,
                                                 VkFilter min_filter,
                                                 VkSamplerAddressMode address_mode_u,
                                                 VkSamplerAddressMode address_mode_v);
  Sampler() = delete;
  Sampler(const Sampler &) = delete;
  [[nodiscard]] VkSampler GetSampler() const;
  ~Sampler();
 private:
  Sampler(const std::shared_ptr<RenderingContext> &context,
          VkFilter mag_filter,
          VkFilter min_filter,
          VkSamplerAddressMode address_mode_u,
          VkSamplerAddressMode address_mode_v);
  VkSampler sampler_ = VK_NULL_HANDLE;
  std::shared_ptr<RenderingContext> context_;
};
}  // namespace vulkan
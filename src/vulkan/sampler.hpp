#pragma once

#include <cstddef>

#include "rendering_context.hpp"

namespace vulkan {
class Sampler final {
 public:
  static std::shared_ptr<Sampler> Create(const std::shared_ptr<RenderingContext> &context, VkFilter mag_filter,
                                         VkFilter min_filter, VkSamplerAddressMode address_mode_u,
                                         VkSamplerAddressMode address_mode_v);
  Sampler() = delete;
  Sampler(const Sampler &) = delete;
  Sampler(Sampler &&) = delete;

  Sampler &operator=(const Sampler &) = delete;
  Sampler &operator=(Sampler &&) = delete;
  [[nodiscard]] VkSampler GetSampler() const;
  ~Sampler();

 private:
  Sampler(const std::shared_ptr<RenderingContext> &context, VkFilter mag_filter, VkFilter min_filter,
          VkSamplerAddressMode address_mode_u, VkSamplerAddressMode address_mode_v);
  VkSampler sampler_ = VK_NULL_HANDLE;
  std::shared_ptr<RenderingContext> context_;
};
}  // namespace vulkan

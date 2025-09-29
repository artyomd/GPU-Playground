#pragma once

#include <memory>

#include "vulkan/compute_pipeline.hpp"
#include "vulkan/rendering_context.hpp"

namespace renderable {

class Blur {
  std::shared_ptr<vulkan::RenderingContext> context_ = nullptr;
  std::shared_ptr<vulkan::ComputePipeline> pipeline_ = nullptr;
  std::shared_ptr<vulkan::ImageView> source_ = nullptr;
  size_t descriptor_set_count_ = 0;
  struct Params {
    float fade = 0;
    bool operator==(const Params& params) const = default;
    [[nodiscard]] std::uint64_t hash() const {
      std::uint64_t seed = 0x3A7CBCA9;
      seed ^= (seed << 6) + (seed >> 2) + 0x2A3FFFD7 + static_cast<std::size_t>(fade);
      return seed;
    }
  };
  std::vector<std::shared_ptr<vulkan::Buffer>> ubos_;
  Params current_;

  explicit Blur(const std::shared_ptr<vulkan::RenderingContext>& context, size_t descriptor_set_count);

 public:
  static std::shared_ptr<Blur> Create(const std::shared_ptr<vulkan::RenderingContext>& context,
                                      size_t descriptor_set_count);
  void RenderParams();
  [[nodiscard]] uint64_t ParamsHash() const;
  [[nodiscard]] std::uint64_t Process(const VkCommandBuffer& command_buffer,
                                      const std::shared_ptr<vulkan::ImageView>& source,
                                      const std::shared_ptr<vulkan::ImageView>& dst, size_t descriptor_index) const;
  [[nodiscard]] size_t GetDescriptorCount() const;
  Blur() = delete;
  Blur(const Blur& other) = delete;
  Blur(Blur&& other) noexcept = delete;
  Blur& operator=(const Blur& other) = delete;
  Blur& operator=(Blur&& other) = delete;
  ~Blur() = default;
};
}  // namespace renderable

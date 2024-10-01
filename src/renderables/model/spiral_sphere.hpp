#pragma once

#include "geometry/spiral_sphere.hpp"
#include "model.hpp"

namespace renderable {
class SpiralSphere final : public Model {
  std::shared_ptr<geometry::SpiralSphere> sphere_ = nullptr;
  std::shared_ptr<vulkan::Shader> v_shader_ = nullptr;
  std::shared_ptr<vulkan::Shader> f_shader_ = nullptr;

  SpiralSphere(std::shared_ptr<vulkan::RenderingContext> context, std::shared_ptr<Menu> parent);
  std::shared_ptr<vulkan::RenderingPipeline> CreatePipeline(
      const std::shared_ptr<vulkan::RenderingContext> &rendering_context,
      const std::shared_ptr<vulkan::RenderPass> &render_pass, const size_t &descriptor_set_count,
      const VkSampleCountFlagBits &sample_count) override;
  size_t NumOfIndicesToDraw() override;

 public:
  static std::shared_ptr<SpiralSphere> Create(std::shared_ptr<vulkan::RenderingContext> context,
                                              std::shared_ptr<Menu> parent);
  SpiralSphere() = delete;
  SpiralSphere(const SpiralSphere &) = delete;
  SpiralSphere(SpiralSphere &&) = delete;
  SpiralSphere &operator=(const SpiralSphere &) = delete;
  SpiralSphere &operator=(SpiralSphere &&) = delete;
  ~SpiralSphere() override;
};
}  // namespace renderable

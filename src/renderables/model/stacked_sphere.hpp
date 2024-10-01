#pragma once

#include "geometry/stacked_sphere.hpp"
#include "model.hpp"

namespace renderable {
class StackedSphere final : public Model {
  std::shared_ptr<geometry::StackedSphere> sphere_ = nullptr;
  std::shared_ptr<vulkan::Shader> v_shader_ = nullptr;
  std::shared_ptr<vulkan::Shader> f_shader_ = nullptr;

  StackedSphere(std::shared_ptr<vulkan::RenderingContext> context, std::shared_ptr<Menu> parent);
  std::shared_ptr<vulkan::RenderingPipeline> CreatePipeline(
      const std::shared_ptr<vulkan::RenderingContext> &rendering_context,
      const std::shared_ptr<vulkan::RenderPass> &render_pass, const size_t &descriptor_set_count,
      const VkSampleCountFlagBits &sample_count) override;
  size_t NumOfIndicesToDraw() override;

 public:
  static std::shared_ptr<StackedSphere> Create(std::shared_ptr<vulkan::RenderingContext> context,
                                               std::shared_ptr<Menu> parent);
  StackedSphere() = delete;
  StackedSphere(const StackedSphere &) = delete;
  StackedSphere(StackedSphere &&) = delete;
  StackedSphere &operator=(const StackedSphere &) = delete;
  StackedSphere &operator=(StackedSphere &&) = delete;
  ~StackedSphere() override;
};
}  // namespace renderable

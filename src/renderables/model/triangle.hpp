#pragma once

#include "geometry/triangle.hpp"
#include "model.hpp"

namespace renderable {
class Triangle final : public Model {
  std::shared_ptr<geometry::Triangle> triangle_ = nullptr;
  std::shared_ptr<vulkan::Shader> v_shader_ = nullptr;
  std::shared_ptr<vulkan::Shader> f_shader_ = nullptr;

  Triangle(const std::shared_ptr<vulkan::RenderingContext> &context, const std::shared_ptr<Menu> &parent);
  std::shared_ptr<vulkan::RenderingPipeline> CreatePipeline(
      const std::shared_ptr<vulkan::RenderingContext> &rendering_context,
      const std::shared_ptr<vulkan::RenderPass> &render_pass, const size_t &descriptor_set_count,
      const VkSampleCountFlagBits &sample_count) override;
  size_t NumOfIndicesToDraw() override;

 public:
  static std::shared_ptr<Triangle> Create(const std::shared_ptr<vulkan::RenderingContext> &context,
                                          const std::shared_ptr<Menu> &parent);
  Triangle() = delete;
  Triangle(const Triangle &) = delete;
  Triangle(Triangle &&) = delete;
  Triangle &operator=(const Triangle &) = delete;
  Triangle &operator=(Triangle &&) = delete;
  ~Triangle() override;
};
}  // namespace renderable


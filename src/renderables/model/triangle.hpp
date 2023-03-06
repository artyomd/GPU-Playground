#pragma once

#include "geometry/triangle.hpp"
#include "model.hpp"

namespace renderable {
class Triangle : public Model {
 private:
  std::shared_ptr<geometry::Triangle> triangle_ = nullptr;
  std::shared_ptr<vulkan::Shader> v_shader_ = nullptr;
  std::shared_ptr<vulkan::Shader> f_shader_ = nullptr;

  Triangle(std::shared_ptr<vulkan::RenderingContext> context, std::shared_ptr<Menu> parent);
  std::shared_ptr<vulkan::RenderingPipeline> CreatePipeline(std::shared_ptr<vulkan::RenderingContext> rendering_context,
                                                            std::shared_ptr<vulkan::RenderPass> render_pass,
                                                            size_t descriptor_set_count,
                                                            VkSampleCountFlagBits sample_count) override;
  size_t NumOfIndicesToDraw() override;
 public:
  static std::shared_ptr<Triangle> Create(std::shared_ptr<vulkan::RenderingContext> context,
                                          std::shared_ptr<Menu> parent);
  Triangle() = delete;
  Triangle(const Triangle &) = delete;
  ~Triangle() override;
};
} // namespace renderable


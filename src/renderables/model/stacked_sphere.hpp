#include "geometry/stacked_sphere.hpp"
#include "model.hpp"

namespace renderable {
class StackedSphere : public Model {
 private:
  std::shared_ptr<geometry::StackedSphere> sphere_ = nullptr;
  std::shared_ptr<vulkan::Shader> v_shader_ = nullptr;
  std::shared_ptr<vulkan::Shader> f_shader_ = nullptr;

  StackedSphere(std::shared_ptr<vulkan::RenderingContext> context, std::shared_ptr<Menu> parent);
  std::shared_ptr<vulkan::RenderingPipeline> CreatePipeline(std::shared_ptr<vulkan::RenderingContext> rendering_context,
                                                            std::shared_ptr<vulkan::RenderPass> render_pass,
                                                            size_t descriptor_set_count,
                                                            VkSampleCountFlagBits sample_count) override;
  size_t NumOfIndicesToDraw() override;
 public:
  static std::shared_ptr<StackedSphere> Create(std::shared_ptr<vulkan::RenderingContext> context,
                                               std::shared_ptr<Menu> parent);
  StackedSphere() = delete;
  StackedSphere(const StackedSphere &) = delete;
  ~StackedSphere() override;
};
} // namespace renderable

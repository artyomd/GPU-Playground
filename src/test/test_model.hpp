#pragma once

#include <glm/glm.hpp>
#include <utility>

#include "src/test/test.h"

namespace test {
struct UniformBufferObjectMvp {
  alignas(16) glm::mat4 model = glm::mat4(1.0F);
  alignas(16) glm::mat4 view = glm::mat4(1.0F);
  alignas(16) glm::mat4 proj = glm::mat4(1.0F);
};

class TestModel : public Test {
 private:
  glm::vec3 translation_ = glm::vec3(0.0F, 0.0F, 0.0F);
  glm::vec3 scale_ = glm::vec3(1.0F, 1.0F, 1.0F);
  glm::vec3 rotate_ = glm::vec3(0.0F, 0.0F, 0.0F);
  float max_side_ = 2.0F;

 protected:
  UniformBufferObjectMvp ubo_{};
  std::shared_ptr<api::RenderingPipeline> pipeline_ = nullptr;
  size_t index_count_ = 0;
  glm::mat4 ComputeModelMatrix();
  glm::mat4 orthographic_projection_{};
  bool lock_projection_ = false;
  glm::mat4 perspective_projection_ = glm::mat4(1.0);

 public:
  explicit TestModel(std::shared_ptr<api::RenderingContext> rendering_context)
      : Test(std::move(rendering_context)) {};

  void OnImGuiRender() override;

  void OnViewportChange(uint32_t width, uint32_t height) override;

  ~TestModel() override = default;
};
}  // namespace test

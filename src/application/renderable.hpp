#pragma once

#include <vector>

#include "include/vulkan_include.hpp"
#include "vulkan/image.hpp"

namespace application {
class Renderable {
 public:
  Renderable() = default;
  Renderable(const Renderable&) = delete;
  Renderable(Renderable&&) = delete;

  Renderable& operator=(const Renderable&) = delete;
  Renderable& operator=(Renderable&&) = delete;

  virtual void SetupImages(const std::vector<std::shared_ptr<vulkan::Image>>& images) = 0;
  virtual VkSemaphore Render(const std::shared_ptr<vulkan::Image>& image, const VkSemaphore& semaphore) = 0;
  virtual ~Renderable() = default;
};
}  // namespace application

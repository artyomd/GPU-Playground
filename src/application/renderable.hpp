#pragma once

#include "vulkan/vulkan_include.hpp"
#include "vulkan/image.hpp"

#include <memory>
#include <vector>
namespace application {
class Renderable {
 public:
  virtual void SetupImages(std::vector<std::shared_ptr<vulkan::Image>> images) = 0;
  virtual VkSemaphore Render(std::shared_ptr<vulkan::Image> image, const VkSemaphore &semaphore) = 0;
  virtual ~Renderable() = default;
};
} // namespace application
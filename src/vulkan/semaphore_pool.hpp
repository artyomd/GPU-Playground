#pragma once

#include <map>
#include <queue>

#include "rendering_context.hpp"

namespace vulkan {
class SemaphorePool {
  std::queue<VkSemaphore> availableSemaphores_;
  std::map<uint32_t, VkSemaphore> semaphoresInQueue_;
  std::shared_ptr<RenderingContext> rendering_context_ = nullptr;

 public:
  SemaphorePool() = delete;
  SemaphorePool(const SemaphorePool &) = delete;
  SemaphorePool(SemaphorePool &&) = delete;

  SemaphorePool &operator=(const SemaphorePool &) = delete;
  SemaphorePool &operator=(SemaphorePool &&) = delete;

  explicit SemaphorePool(const std::shared_ptr<RenderingContext> &rendering_context)
      : rendering_context_(rendering_context) {};

  VkSemaphore GetSemaphore();
  void RegisterSemaphore(uint32_t index, const VkSemaphore &semaphore);

  ~SemaphorePool();
};
}  // namespace vulkan

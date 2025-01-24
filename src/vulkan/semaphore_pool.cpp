#include "semaphore_pool.hpp"

#include <ranges>

VkSemaphore vulkan::SemaphorePool::GetSemaphore() {
  if (availableSemaphores_.empty()) {
    return rendering_context_->CreateSemaphore();
  }
  auto* semaphore = availableSemaphores_.front();
  availableSemaphores_.pop();
  return semaphore;
}

void vulkan::SemaphorePool::RegisterSemaphore(const uint32_t index, const VkSemaphore& semaphore) {
  if (semaphoresInQueue_.contains(index)) {
    availableSemaphores_.push(semaphoresInQueue_[index]);
  }
  semaphoresInQueue_[index] = semaphore;
}
vulkan::SemaphorePool::~SemaphorePool() {
  rendering_context_->WaitForGraphicsQueueIdle();
  while (!availableSemaphores_.empty()) {
    rendering_context_->DestroySemaphore(availableSemaphores_.front());
    availableSemaphores_.pop();
  }
  for (auto* semaphore : semaphoresInQueue_ | std::views::values) {
    rendering_context_->DestroySemaphore(semaphore);
  }
}

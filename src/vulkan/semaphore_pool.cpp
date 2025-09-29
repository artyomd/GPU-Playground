#include "semaphore_pool.hpp"

#include <ranges>

VkSemaphore vulkan::SemaphorePool::GetSemaphore() {
  if (available_semaphores_.empty()) {
    return rendering_context_->CreateSemaphore();
  }
  auto* semaphore = available_semaphores_.front();
  available_semaphores_.pop();
  return semaphore;
}

void vulkan::SemaphorePool::RegisterSemaphore(const uint32_t index, const VkSemaphore& semaphore) {
  if (semaphores_in_queue_.contains(index)) {
    available_semaphores_.push(semaphores_in_queue_[index]);
  }
  semaphores_in_queue_[index] = semaphore;
}
vulkan::SemaphorePool::~SemaphorePool() {
  rendering_context_->WaitForQueueIdle();
  while (!available_semaphores_.empty()) {
    rendering_context_->DestroySemaphore(available_semaphores_.front());
    available_semaphores_.pop();
  }
  for (auto* semaphore : semaphores_in_queue_ | std::views::values) {
    rendering_context_->DestroySemaphore(semaphore);
  }
}

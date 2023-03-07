#pragma once

#include "vulkan_include.hpp"

#include <condition_variable>
#include <memory>
#include <mutex>

namespace vulkan {
class RenderingContext {
 public:
  explicit RenderingContext(VkInstance instance,
                            VkPhysicalDevice physical_device,
                            VkDevice device,
                            VkQueue graphics_queue,
                            uint32_t queue_family_index,
                            bool use_sync2_ext);

  [[nodiscard]] VkInstance GetInstance() const;

  [[nodiscard]] VkPhysicalDevice GetPhysicalDevice() const;

  [[nodiscard]] VkQueue GetGraphicsQueue() const;

  [[nodiscard]] VkDevice GetDevice() const;

  [[nodiscard]] VkDescriptorPool GetDescriptorPool() const;

  [[nodiscard]] VmaAllocator GetAllocator() const;

  [[nodiscard]]  VkCommandPool CreateCommandPool(VkCommandPoolCreateFlags pool_flags = 0);

  [[nodiscard]]  VkCommandBuffer CreateCommandBuffer(VkCommandPool command_pool);

  void SubmitCommandBuffer(const VkCommandBuffer &command_buffer,
                           const VkSemaphore &wait_semaphore = VK_NULL_HANDLE,
                           const VkPipelineStageFlags2 wait_dst_stage_mask = VK_PIPELINE_STAGE_2_NONE,
                           const VkSemaphore &signal_semaphore = VK_NULL_HANDLE,
                           VkFence fence = VK_NULL_HANDLE);

  void FreeCommandBuffer(VkCommandPool command_pool, VkCommandBuffer buffer);

  void DestroyCommandPool(VkCommandPool command_pool);

  [[nodiscard]] VkFence CreateFence(bool create_signaled = false);

  void WaitForFence(VkFence fence);

  void ResetFence(VkFence fence);

  void DestroyFence(VkFence fence);

  [[nodiscard]] VkSemaphore CreateSemaphore();

  void DestroySemaphore(VkSemaphore semaphore);

  void WaitForGpuIdle() const;

  bool IsUseSynch2Ext() const;

  virtual ~RenderingContext();
 private:
  VkInstance instance_ = VK_NULL_HANDLE;
  VkPhysicalDevice physical_device_ = VK_NULL_HANDLE;
  VkDevice device_ = VK_NULL_HANDLE;
  VkQueue graphics_queue_ = VK_NULL_HANDLE;
  uint32_t queue_family_index_ = 0;
  VkDescriptorPool descriptor_pool_ = VK_NULL_HANDLE;
  VmaAllocator allocator_ = VK_NULL_HANDLE;
  bool use_synch2_ext_ = false;
};
}  // namespace vulkan
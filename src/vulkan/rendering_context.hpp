#pragma once

#include <condition_variable>
#include <mutex>

#include "include/vulkan_include.hpp"

namespace vulkan {
class RenderingContext final {
 public:
  RenderingContext() = delete;
  RenderingContext(const RenderingContext&) = delete;
  RenderingContext(RenderingContext&&) = delete;
  explicit RenderingContext(const VkInstance& instance, const VkPhysicalDevice& physical_device, const VkDevice& device,
                            const VkQueue& graphics_queue, uint32_t queue_family_index, bool use_sync_2_ext);

  RenderingContext& operator=(const RenderingContext&) = delete;
  RenderingContext& operator=(RenderingContext&&) = delete;

  [[nodiscard]] VkInstance GetInstance() const;

  [[nodiscard]] VkPhysicalDevice GetPhysicalDevice() const;

  [[nodiscard]] VkQueue GetGraphicsQueue() const;

  [[nodiscard]] VkDevice GetDevice() const;

  [[nodiscard]] VkDescriptorPool GetDescriptorPool() const;

  [[nodiscard]] VmaAllocator GetAllocator() const;

  [[nodiscard]] VkCommandPool CreateCommandPool(VkCommandPoolCreateFlags pool_flags = 0) const;

  [[nodiscard]] VkCommandBuffer CreateCommandBuffer(const VkCommandPool& command_pool) const;

  void SubmitCommandBuffer(const VkCommandBuffer& command_buffer, const VkSemaphore& wait_semaphore = VK_NULL_HANDLE,
                           VkPipelineStageFlags2 wait_dst_stage_mask = VK_PIPELINE_STAGE_2_NONE,
                           const VkSemaphore& signal_semaphore = VK_NULL_HANDLE,
                           const VkFence& fence = VK_NULL_HANDLE) const;

  void FreeCommandBuffer(const VkCommandPool& command_pool, const VkCommandBuffer& command_buffer) const;

  void DestroyCommandPool(const VkCommandPool& command_pool) const;

  [[nodiscard]] VkFence CreateFence(bool create_signaled = false) const;

  void WaitForFence(const VkFence& fence) const;

  void ResetFence(const VkFence& fence) const;

  void DestroyFence(const VkFence& fence) const;

  [[nodiscard]] VkSemaphore CreateSemaphore() const;

  void DestroySemaphore(const VkSemaphore& semaphore) const;

  void WaitForGraphicsQueueIdle() const;

  [[nodiscard]] bool IsUseSynch2Ext() const;

  [[nodiscard]] uint32_t GetPhysicalDeviceVkSpecVersion() const;

  ~RenderingContext();

 private:
  VkInstance instance_ = VK_NULL_HANDLE;
  VkPhysicalDevice physical_device_ = VK_NULL_HANDLE;
  uint32_t physical_device_vk_spec_version_ = VK_API_VERSION_1_0;
  VkDevice device_ = VK_NULL_HANDLE;
  VkQueue graphics_queue_ = VK_NULL_HANDLE;
  uint32_t queue_family_index_ = 0;
  VkDescriptorPool descriptor_pool_ = VK_NULL_HANDLE;
  VmaAllocator allocator_ = VK_NULL_HANDLE;
  bool use_synch2_ext_ = false;
};
}  // namespace vulkan

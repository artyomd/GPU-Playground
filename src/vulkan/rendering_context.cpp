#include "rendering_context.hpp"

#include "utils.hpp"

#include <tracy/Tracy.hpp>

#include <stdexcept>
#include <vector>

vulkan::RenderingContext::RenderingContext(VkInstance instance,
                                           VkPhysicalDevice physical_device,
                                           VkDevice device,
                                           VkQueue graphics_queue,
                                           uint32_t queue_family_index,
                                           bool use_sync2_ext) : instance_(instance),
                                                                 physical_device_(physical_device),
                                                                 device_(device),
                                                                 graphics_queue_(graphics_queue),
                                                                 queue_family_index_(queue_family_index),
                                                                 use_synch2_ext_(use_sync2_ext) {

  std::vector<VkDescriptorPoolSize> pool_sizes =
      {{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 50},
       {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 50},
       {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 50}};
  VkDescriptorPoolCreateInfo descriptor_pool_create_info = {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
      .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
      .maxSets = 50 * static_cast<uint32_t>(pool_sizes.size()),
      .poolSizeCount = static_cast<uint32_t>(pool_sizes.size()),
      .pPoolSizes = pool_sizes.data(),
  };

  VK_CALL(vkCreateDescriptorPool(device_, &descriptor_pool_create_info, nullptr, &descriptor_pool_));

  VmaVulkanFunctions functions{
      .vkGetInstanceProcAddr = vkGetInstanceProcAddr,
      .vkGetDeviceProcAddr = vkGetDeviceProcAddr,
      .vkGetPhysicalDeviceProperties = vkGetPhysicalDeviceProperties,
      .vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties,
      .vkAllocateMemory = vkAllocateMemory,
      .vkFreeMemory = vkFreeMemory,
      .vkMapMemory = vkMapMemory,
      .vkUnmapMemory = vkUnmapMemory,
      .vkFlushMappedMemoryRanges = vkFlushMappedMemoryRanges,
      .vkInvalidateMappedMemoryRanges = vkInvalidateMappedMemoryRanges,
      .vkBindBufferMemory = vkBindBufferMemory,
      .vkBindImageMemory = vkBindImageMemory,
      .vkGetBufferMemoryRequirements = vkGetBufferMemoryRequirements,
      .vkGetImageMemoryRequirements = vkGetImageMemoryRequirements,
      .vkCreateBuffer = vkCreateBuffer,
      .vkDestroyBuffer = vkDestroyBuffer,
      .vkCreateImage = vkCreateImage,
      .vkDestroyImage = vkDestroyImage,
      .vkCmdCopyBuffer = vkCmdCopyBuffer,
      .vkGetBufferMemoryRequirements2KHR = vkGetBufferMemoryRequirements2,
      .vkGetImageMemoryRequirements2KHR = vkGetImageMemoryRequirements2,
      .vkBindBufferMemory2KHR = vkBindBufferMemory2,
      .vkBindImageMemory2KHR = vkBindImageMemory2,
      .vkGetPhysicalDeviceMemoryProperties2KHR = vkGetPhysicalDeviceMemoryProperties2,
      .vkGetDeviceBufferMemoryRequirements = vkGetDeviceBufferMemoryRequirements,
      .vkGetDeviceImageMemoryRequirements = vkGetDeviceImageMemoryRequirements,
  };

  VmaDeviceMemoryCallbacks device_memory_callbacks = {
      .pfnAllocate = [](VmaAllocator,
                        uint32_t,
                        VkDeviceMemory VMA_NOT_NULL_NON_DISPATCHABLE memory,
                        VkDeviceSize size,
                        void *) {
        TracyAllocN(memory, size, "vulkan");
      },
      .pfnFree = [](VmaAllocator,
                    uint32_t,
                    VkDeviceMemory VMA_NOT_NULL_NON_DISPATCHABLE memory,
                    VkDeviceSize size,
                    void *VMA_NULLABLE) {
        TracyFreeN(memory, "vulkan");
      },
  };

  VmaAllocatorCreateInfo allocator_info{
      .flags = 0,
      .physicalDevice = physical_device_,
      .device = device_,
      .preferredLargeHeapBlockSize = 0,
      .pAllocationCallbacks = nullptr,
      .pDeviceMemoryCallbacks = &device_memory_callbacks,
      .pHeapSizeLimit = nullptr,
      .pVulkanFunctions = &functions,
      .instance = instance_,
      .vulkanApiVersion = GetDeviceApiVersion(physical_device_),
      .pTypeExternalMemoryHandleTypes = nullptr,
  };

  VK_CALL(vmaCreateAllocator(&allocator_info, &allocator_));
}

VkDevice vulkan::RenderingContext::GetDevice() const {
  return device_;
}

void vulkan::RenderingContext::WaitForGpuIdle() const {
  VK_CALL(vkDeviceWaitIdle(device_));
}

VkCommandPool vulkan::RenderingContext::CreateCommandPool(VkCommandPoolCreateFlags pool_flags) {
  VkCommandPool command_pool = VK_NULL_HANDLE;
  VkCommandPoolCreateInfo pool_info = {
      .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
      .flags = pool_flags,
      .queueFamilyIndex = queue_family_index_,
  };
  VK_CALL(vkCreateCommandPool(device_, &pool_info, nullptr, &command_pool));
  return command_pool;
}

VkCommandBuffer vulkan::RenderingContext::CreateCommandBuffer(VkCommandPool command_pool) {
  VkCommandBufferAllocateInfo alloc_info = {
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      .commandPool = command_pool,
      .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
      .commandBufferCount = 1,
  };
  VkCommandBuffer command_buffer = VK_NULL_HANDLE;
  VK_CALL(vkAllocateCommandBuffers(device_, &alloc_info, &command_buffer));
  return command_buffer;
}

void vulkan::RenderingContext::SubmitCommandBuffer(const VkCommandBuffer &command_buffer,
                                                   const VkSemaphore &wait_semaphore,
                                                   const VkPipelineStageFlags2 wait_dst_stage_mask,
                                                   const VkSemaphore &signal_semaphore,
                                                   VkFence fence) {
  VkSemaphoreSubmitInfo wait_semaphore_submit_info{
      .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
      .pNext = nullptr,
      .semaphore = wait_semaphore,
      .stageMask = wait_dst_stage_mask,
      .deviceIndex = 0,
  };
  VkSemaphoreSubmitInfo signal_semaphore_submit_info{
      .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
      .pNext = nullptr,
      .semaphore = signal_semaphore,
      .stageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
      .deviceIndex = 0,
  };
  VkCommandBufferSubmitInfo command_buffer_submit_info{
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
      .commandBuffer = command_buffer,
  };
  VkSubmitInfo2 submit_info = {
      .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
      .waitSemaphoreInfoCount = wait_semaphore == VK_NULL_HANDLE ? 0u : 1u,
      .pWaitSemaphoreInfos = &wait_semaphore_submit_info,
      .commandBufferInfoCount = 1,
      .pCommandBufferInfos = &command_buffer_submit_info,
      .signalSemaphoreInfoCount = signal_semaphore == VK_NULL_HANDLE ? 0u : 1u,
      .pSignalSemaphoreInfos = &signal_semaphore_submit_info
  };
  if (use_synch2_ext_) {
    VK_CALL(vkQueueSubmit2KHR(graphics_queue_, 1, &submit_info, fence));
  } else {
    VK_CALL(vkQueueSubmit2(graphics_queue_, 1, &submit_info, fence));
  }
}

void vulkan::RenderingContext::FreeCommandBuffer(VkCommandPool command_pool, VkCommandBuffer command_buffer) {
  vkFreeCommandBuffers(device_, command_pool, 1, &command_buffer);
}

void vulkan::RenderingContext::DestroyCommandPool(VkCommandPool command_pool) {
  vkResetCommandPool(device_, command_pool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);
  vkDestroyCommandPool(device_, command_pool, nullptr);
}

vulkan::RenderingContext::~RenderingContext() {
  vmaDestroyAllocator(allocator_);
  vkDestroyDescriptorPool(device_, descriptor_pool_, nullptr);
}

VkDescriptorPool vulkan::RenderingContext::GetDescriptorPool() const {
  return descriptor_pool_;
}

VmaAllocator vulkan::RenderingContext::GetAllocator() const {
  return allocator_;
}

VkFence vulkan::RenderingContext::CreateFence(bool create_signaled) {
  VkFenceCreateInfo fence_create_info{
      .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
      .flags = create_signaled ? VK_FENCE_CREATE_SIGNALED_BIT : static_cast<VkFenceCreateFlags>(0),
  };
  VkFence fence = VK_NULL_HANDLE;
  VK_CALL(vkCreateFence(device_, &fence_create_info, nullptr, &fence));
  return fence;
}

void vulkan::RenderingContext::WaitForFence(VkFence fence) {
  VK_CALL(vkWaitForFences(device_, 1, &fence, true, std::numeric_limits<uint64_t>::max()));
}

void vulkan::RenderingContext::ResetFence(VkFence fence) {
  VK_CALL(vkResetFences(device_, 1, &fence));
}

void vulkan::RenderingContext::DestroyFence(VkFence fence) {
  vkDestroyFence(device_, fence, nullptr);
}

VkSemaphore vulkan::RenderingContext::CreateSemaphore() {
  VkSemaphoreCreateInfo semaphore_create_info{
      .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
  };
  VkSemaphore semaphore = VK_NULL_HANDLE;
  VK_CALL(vkCreateSemaphore(device_, &semaphore_create_info, nullptr, &semaphore));
  return semaphore;
}

void vulkan::RenderingContext::DestroySemaphore(VkSemaphore semaphore) {
  vkDestroySemaphore(device_, semaphore, nullptr);
}

VkInstance vulkan::RenderingContext::GetInstance() const {
  return instance_;
}

VkPhysicalDevice vulkan::RenderingContext::GetPhysicalDevice() const {
  return physical_device_;
}

VkQueue vulkan::RenderingContext::GetGraphicsQueue() const {
  return graphics_queue_;
}

bool vulkan::RenderingContext::IsUseSynch2Ext() const {
  return use_synch2_ext_;
}

//
// Created by artyomd on 12/7/19.
//

#pragma once

#define GLFW_INCLUDE_VULKAN

#include "GlfwApplication.h"
#include "api/vulkan/VkRenderingContext.h"
#include <vulkan/vulkan.h>

namespace application {
class VulkanApplication : public GlfwApplication {
 private:
  api::VkRenderingContext *context_ = nullptr;
#ifdef NDEBUG
  const bool enable_validation_layers_ = false;
#else
  const bool enable_validation_layers_ = true;
#endif
  const std::vector<const char *> validation_layers_ = {
      "VK_LAYER_KHRONOS_validation"
  };

  const std::vector<const char *> device_extensions_ = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME
  };

  const int max_frames_in_flight_ = 2;

  VkInstance vulkan_instance_;
  VkDebugUtilsMessengerEXT debug_messenger_;

  VkSurfaceKHR surface_;
  VkPhysicalDevice physical_device_ = VK_NULL_HANDLE;
  VkDevice device_;

  VkRenderPass render_pass_;

  VkQueue graphics_queue_;
  VkCommandPool graphics_command_pool_;
  std::vector<VkCommandBuffer> graphics_command_buffers_;

  VkQueue present_queue_;

  VkSwapchainKHR swap_chain_;
  VkExtent2D swap_chain_extent_;
  VkFormat swap_chain_image_format_;
  std::vector<VkImage> swap_chain_images_;
  std::vector<VkImageView> swap_chain_image_views_;
  std::vector<VkFramebuffer> swap_chain_frame_buffers_;

  VkDescriptorPool descriptor_pool_;

  std::vector<VkSemaphore> image_available_semaphores_;
  std::vector<VkSemaphore> render_finished_semaphores_;
  std::vector<VkFence> in_flight_fences_;
  std::vector<VkFence> images_in_flight_;

  bool framebuffer_resized_ = false;
  int current_frame_ = 0;
  uint32_t current_image_ = 0;

  struct QueueFamilyIndices {
    std::optional<uint32_t> graphics_family_;
    std::optional<uint32_t> present_family_;

    bool IsComplete() {
      return graphics_family_.has_value() &&
          present_family_.has_value();
    }
  };

  struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities_{};
    std::vector<VkSurfaceFormatKHR> formats_;
    std::vector<VkPresentModeKHR> present_modes_;
  };

  void CreateInstance();

  void RecreateSwapChain();

  bool CheckValidationLayerSupport();

  std::vector<const char *> GetRequiredExtensions();

  void SetupDebugMessenger();

  void CreateSurface();

  bool IsDeviceSuitable(VkPhysicalDevice device);

  void PickPhysicalDevice();

  QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);

  bool CheckDeviceExtensionSupport(VkPhysicalDevice device);

  SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);

  void CreateLogicalDevice();

  void CreateDescriptorPool();

  void CreateCommandPool();

  void CreateSwapChain();

  static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &available_formats);

  static VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &available_present_modes);

  VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

  void CreateImageViews();

  void CreateRenderPass();

  void CreateFramebuffers();

  void CreateCommandBuffers();

  void CreateSyncObjects();

  void CleanupSwapChain();

 protected:
  void SetupWindowHints() final;

  void OnWindowSizeChanged() final;

  void InitImGui() final;

  bool PrepareFrame() final;

  void CreateImGuiFrame() final;

  void RenderImGui() final;

  void DrawFrame() final;

  void DestroyImGui() final;

  void PrepareForShutdown() final;

 public:
  void InitContext() final;

  void DestroyContext() override;
};
}

#pragma once

#define GLFW_INCLUDE_VULKAN
#include <optional>

#include "src/api/vulkan/vulkan_rendering_context.hpp"
#include "src/application/glfw_application.hpp"

namespace application {
class VulkanApplication : public GlfwApplication {
 private:
  const std::vector<const char *> device_extensions_ = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME};

  const uint32_t max_frames_in_flight_ = 2;

  VkInstance vulkan_instance_ = VK_NULL_HANDLE;
  VkDebugUtilsMessengerEXT debug_messenger_ = VK_NULL_HANDLE;

  VkSurfaceKHR surface_{};
  VkPhysicalDevice physical_device_ = VK_NULL_HANDLE;
  VkDevice device_{};

  VkQueue graphics_queue_{};
  VkCommandPool graphics_command_pool_{};
  std::vector<VkCommandBuffer> graphics_command_buffers_;

  VkQueue present_queue_{};

  VkSwapchainKHR swap_chain_{};
  VkExtent2D swap_chain_extent_{};
  VkFormat swap_chain_image_format_ = VK_FORMAT_UNDEFINED;
  std::vector<VkImage> swap_chain_images_;
  std::vector<VkImageView> swap_chain_image_views_;
  std::vector<VkFramebuffer> swap_chain_frame_buffers_;

  VkImage color_image_{};
  VkDeviceMemory color_image_memory_{};
  VkImageView color_image_view_{};

  VkImage depth_image_{};
  VkDeviceMemory depth_image_memory_{};
  VkImageView depth_image_view_{};

  std::vector<VkSemaphore> image_available_semaphores_;
  std::vector<VkSemaphore> render_finished_semaphores_;
  std::vector<VkFence> in_flight_fences_;
  std::vector<VkFence> images_in_flight_;

  bool framebuffer_resized_ = false;
  bool framebuffer_empty_ = false;

  uint32_t current_frame_ = 0;
  uint32_t current_image_ = 0;

  struct QueueFamilyIndices {
    std::optional<uint32_t> graphics_family;
    std::optional<uint32_t> present_family;

    [[nodiscard]] bool IsComplete() const {
      return graphics_family.has_value() && present_family.has_value();
    }
  };

  struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities{};
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> present_modes;
  };

  std::shared_ptr<api::vulkan::VulkanRenderingContext> GetVulkanContext();

  void CreateInstance();

  void RecreateSwapChain();

  void CreateSurface();

  bool IsDeviceSuitable(VkPhysicalDevice device);

  void PickPhysicalDevice();

  QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);

  bool CheckDeviceExtensionSupport(VkPhysicalDevice device);

  SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);

  void CreateLogicalDevice();

  void CreateCommandPool();

  void CreateSwapChain();

  static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR> &available_formats,
      VkFormat desired_format);

  static VkPresentModeKHR ChooseSwapPresentMode(
      const std::vector<VkPresentModeKHR> &available_present_modes);

  VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

  void CreateImageViews();

  void CreateColorResources();

  void CreateDepthResources();

  void CreateFrameBuffers();

  void CreateCommandBuffers();

  void CreateSyncObjects();

  void CleanupSwapChain();

  void InitImGui();

  static void DestroyImGui();

 protected:
  void InitContext() final;

  void SetupWindowHints() final;

  void OnWindowSizeChanged(int width, int height) final;

  bool PrepareFrame() final;

  void PrepareImGuiFrame() final;

  void RenderImGui() final;

  void RenderFrame() final;

  void DestroyContext() final;

 public:
  VulkanApplication();
};
}  // namespace application

#pragma once

#include <functional>
#include <map>
#include <vector>

#include "application/renderable.hpp"
#include "include/vulkan_glfw.hpp"
#include "include/vulkan_include.hpp"
#include "vulkan/image.hpp"
#include "vulkan/rendering_context.hpp"
#include "vulkan/semaphore_pool.hpp"

namespace application {
class Application final {
  std::vector<const char *> required_device_extensions_ = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

  VkInstance vulkan_instance_ = VK_NULL_HANDLE;
  VkDebugUtilsMessengerEXT debug_messenger_ = VK_NULL_HANDLE;

  GLFWwindow *window_ = nullptr;
  int window_width_ = 640;
  int window_height_ = 480;
  VkSurfaceKHR surface_ = VK_NULL_HANDLE;

  VkPhysicalDevice physical_device_ = VK_NULL_HANDLE;
  VkDevice device_ = VK_NULL_HANDLE;

  VkQueue graphics_queue_ = VK_NULL_HANDLE;
  uint32_t graphics_family_index_ = 0;

  VkQueue present_queue_ = VK_NULL_HANDLE;
  uint32_t present_family_index_ = 0;

  VkSwapchainKHR swap_chain_ = VK_NULL_HANDLE;

  std::vector<std::shared_ptr<vulkan::Image>> swap_chain_images_;
  std::queue<VkFence> fences_;
  std::vector<VkSemaphore> semaphores_;

  std::shared_ptr<vulkan::RenderingContext> rendering_context_ = nullptr;
  std::shared_ptr<vulkan::SemaphorePool> semaphore_pool_ = nullptr;
  std::shared_ptr<Renderable> renderable_ = nullptr;

  void OnWindowSizeChanged(int width, int height);
  void CreateInstance();
  void PickPhysicalDevice();
  void CreateLogicalDevice();
  void CreateSwapChain();
  void CleanupSwapChain();

 public:
  Application() = delete;
  Application(const Application &) = delete;
  Application(Application &&) = delete;
  Application &operator=(const Application &) = delete;
  Application &operator=(Application &&) = delete;
  explicit Application(
      const std::function<std::shared_ptr<Renderable>(std::shared_ptr<vulkan::RenderingContext>)> &instantiate);
  void Run();
  void RequestExit() const;
  ~Application();
};
}  // namespace application

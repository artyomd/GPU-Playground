#include "application/application.hpp"

#include "vulkan/rendering_context.hpp"
#include "vulkan/utils.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <spdlog/spdlog.h>

#include <array>
#include <optional>
#include <set>
#include <stdexcept>

namespace {
std::vector<const char *> required_device_extentions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

struct QueueFamilyIndices {
  std::optional<uint32_t> graphics_family;
  std::optional<uint32_t> present_family;
  QueueFamilyIndices(VkPhysicalDevice device, VkSurfaceKHR surface) {
    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);
    std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());
    uint32_t i = 0;
    for (const auto &kQueueFamily : queue_families) {
      if (!graphics_family.has_value()) {
        if (kQueueFamily.queueCount > 0 && ((kQueueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0u)) {
          graphics_family = i;
        }
      }
      if (!present_family.has_value()) {
        VkBool32 present_support = VK_FALSE;
        VK_CALL(vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &present_support));
        if (kQueueFamily.queueCount > 0 && (present_support == VK_TRUE)) {
          present_family = i;
        }
      }
      if (IsComplete()) {
        break;
      }
      i++;
    }
  }
  [[nodiscard]] bool IsComplete() const {
    return graphics_family.has_value() && present_family.has_value();
  }
};

struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities{};
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> present_modes;
  SwapChainSupportDetails(VkPhysicalDevice device, VkSurfaceKHR surface) {
    VK_CALL(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &capabilities));
    uint32_t format_count = 0;
    VK_CALL(vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, nullptr));
    formats.resize(format_count);
    VK_CALL(vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, formats.data()));
    uint32_t present_mode_count = 0;
    VK_CALL(vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, nullptr));
    present_modes.resize(present_mode_count);
    VK_CALL(vkGetPhysicalDeviceSurfacePresentModesKHR(device,
                                                      surface,
                                                      &present_mode_count,
                                                      present_modes.data()));
  }
  VkSurfaceFormatKHR ChooseSwapSurfaceFormat(VkFormat desired_format) {
    for (const auto &kAvailableFormat : formats) {
      if (kAvailableFormat.format == desired_format
          && kAvailableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
        return kAvailableFormat;
      }
    }
    return formats[0];
  }

  VkPresentModeKHR ChooseSwapPresentMode() {
    bool mailbox_supported = std::any_of(present_modes.begin(),
                                         present_modes.end(),
                                         [](auto element) { return element == VK_PRESENT_MODE_MAILBOX_KHR; });
    return mailbox_supported ? VK_PRESENT_MODE_MAILBOX_KHR : VK_PRESENT_MODE_FIFO_KHR;
  }
};

std::vector<const char *> GetGlfwExtensions() {
  uint32_t glfw_extension_count = 0;
  const char **glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
  std::vector<const char *> extensions(glfw_extensions, glfw_extensions + glfw_extension_count);
  return extensions;
}
}  // namespace

application::Application::Application(std::function<std::shared_ptr<Renderable>(std::shared_ptr<vulkan::RenderingContext>)> instantiate) {
  glfwSetErrorCallback([](int error, const char *description) {
    throw std::runtime_error("glfw error " + std::to_string(error) + ": " + description);
  });

  if (glfwInit() != GLFW_TRUE) {
    throw std::runtime_error("can not init glfw");
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_FOCUSED, GLFW_FALSE);
  window_ = glfwCreateWindow(static_cast<int>(window_width_),
                             static_cast<int>(window_height_),
                             "GPU Playground",
                             nullptr,
                             nullptr);
  if (window_ == nullptr) {
    glfwTerminate();
    throw std::runtime_error("could not create window");
  }
  glfwSetWindowUserPointer(window_, this);
  glfwSetFramebufferSizeCallback(window_, [](auto window, auto width, auto height) {
    reinterpret_cast<decltype(this)>(glfwGetWindowUserPointer(window))->OnWindowSizeChanged(width, height);
  });
  CreateInstance();
  if (glfwCreateWindowSurface(vulkan_instance_, window_, nullptr, &surface_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create window surface!");
  }
  PickPhysicalDevice();
  CreateLogicalDevice();

  bool use_synch2_ext = std::find(required_device_extentions.begin(),
                                  required_device_extentions.end(),
                                  VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME) != required_device_extentions.end();
  rendering_context_ = std::make_shared<vulkan::RenderingContext>(vulkan_instance_,
                                                                  physical_device_,
                                                                  device_,
                                                                  graphics_queue_,
                                                                  graphics_family_index_,
                                                                  use_synch2_ext);

  CreateSwapChain();
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui_ImplGlfw_InitForVulkan(window_, true);
  renderable_ = instantiate(rendering_context_);
  renderable_->SetupImages(swap_chain_images_);
}

void application::Application::OnWindowSizeChanged(int width, int height) {
  this->window_width_ = width;
  this->window_height_ = height;
  rendering_context_->WaitForGpuIdle();
  CleanupSwapChain();
  if (width == 0 || height == 0) {
    glfwWaitEvents();
    return;
  }
  CreateSwapChain();
  if (renderable_ != nullptr) {
    renderable_->SetupImages(swap_chain_images_);
  }
}

void application::Application::CreateInstance() {
  VK_CALL(volkInitialize());

  uint32_t instance_version = VK_API_VERSION_1_0;
  VK_CALL(vkEnumerateInstanceVersion(&instance_version));
  spdlog::debug("instance version queried {}.{}.{}",
                VK_VERSION_MAJOR(instance_version),
                VK_VERSION_MINOR(instance_version),
                VK_VERSION_PATCH(instance_version));

  VkApplicationInfo app_info{
      .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .pApplicationName= "fantastic-spoon",
      .applicationVersion = 0U,
      .pEngineName = "No Engine",
      .engineVersion = 0U,
      .apiVersion =  instance_version,
  };

  std::vector<const char *> layers{};
  std::vector<const char *> instance_extensions{};

  bool debug_utils_enabled = false;
  bool validation_features_enabled = false;
  void *instance_create_info_next = nullptr;
  std::vector<VkValidationFeatureEnableEXT> enabled_features{
      VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_EXT,
      VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_RESERVE_BINDING_SLOT_EXT,
      VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT,
      VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT,
  };
  VkValidationFeaturesEXT validation_features_ext{
      .sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT,
      .enabledValidationFeatureCount = static_cast<uint32_t>(enabled_features.size()),
      .pEnabledValidationFeatures = enabled_features.data(),
      .disabledValidationFeatureCount = 0, .pDisabledValidationFeatures = nullptr,
  };

  auto available_layers = vulkan::GetAvailableInstanceLayers();
  for (const auto &kLayer : available_layers) {
#if !defined(NDEBUG)
    if (strcmp(kLayer.layerName, "VK_LAYER_KHRONOS_validation") == 0) {
      layers.emplace_back("VK_LAYER_KHRONOS_validation");
    }
#endif
    if (strcmp(kLayer.layerName, "VK_LAYER_KHRONOS_synchronization2") == 0) {
      layers.emplace_back("VK_LAYER_KHRONOS_synchronization2");
    }
  }
#if !defined(NDEBUG)
  if (std::find(layers.begin(), layers.end(), "VK_LAYER_KHRONOS_validation") != layers.end()) {
    auto available_extensions = vulkan::GetAvailableInstanceExtensions("VK_LAYER_KHRONOS_validation");
    for (const auto &kExt : available_extensions) {
      if (strcmp(kExt.extensionName, VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == 0) {
        instance_extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        debug_utils_enabled = true;
      } else if (strcmp(kExt.extensionName, VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME) == 0) {
        instance_extensions.emplace_back(VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME);
        validation_features_enabled = true;
      }
    }
  }
  if (validation_features_enabled) {
    instance_create_info_next = &validation_features_ext;
  }
#endif

  auto available_instance_extensions = vulkan::GetAvailableInstanceExtensions("");
  if (std::find_if(available_instance_extensions.begin(), available_instance_extensions.end(), [](const auto &val) {
    return strcmp(val.extensionName, VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME) == 0;
  }) != available_instance_extensions.end()) {
    instance_extensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
  }

  auto glfw_ext = GetGlfwExtensions();
  instance_extensions.insert(std::end(instance_extensions), std::begin(glfw_ext), std::end(glfw_ext));

  VkInstanceCreateInfo instance_create_info{
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .pNext = instance_create_info_next,
      .flags =(std::find(instance_extensions.begin(),
                         instance_extensions.end(),
                         VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME) != instance_extensions.end())
              ? VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR
              : 0U,
      .pApplicationInfo = &app_info,
      .enabledLayerCount = static_cast<uint32_t>(layers.size()),
      .ppEnabledLayerNames = layers.empty() ? nullptr : layers.data(),
      .enabledExtensionCount = static_cast<uint32_t>(instance_extensions.size()),
      .ppEnabledExtensionNames = instance_extensions.empty() ? nullptr : instance_extensions.data(),
  };

  VK_CALL(vkCreateInstance(&instance_create_info, nullptr, &vulkan_instance_));
  volkLoadInstance(vulkan_instance_);

  if (debug_utils_enabled) {
    VkDebugUtilsMessengerCreateInfoEXT create_info{
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .pNext = nullptr,
        .messageSeverity =VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
            | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
            | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
            | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
            | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback =  [](VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                               VkDebugUtilsMessageTypeFlagsEXT flags,
                               const VkDebugUtilsMessengerCallbackDataEXT *p_callback_data,
                               void *) -> VKAPI_ATTR VkBool32 VKAPI_CALL {
          auto flag_to_string = [](VkDebugUtilsMessageTypeFlagsEXT flag) {
            std::string flags;
            if ((flag & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) != 0u) {
              flags += "|GENERAL";
            }
            if ((flag & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) != 0u) {
              flags += "|VALIDATION";
            }
            if ((flag & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) != 0U) {
              flags += "|PERFORMANCE";
            }
            flags += "|";
            return flags;
          };
          if ((message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) != 0) {
            spdlog::info("validation layer {}: {}", flag_to_string(flags), p_callback_data->pMessage);
          } else if ((message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) != 0) {
            spdlog::warn("validation layer {}: {}", flag_to_string(flags), p_callback_data->pMessage);
          } else if ((message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) != 0) {
            spdlog::error("validation layer {}: {}", flag_to_string(flags), p_callback_data->pMessage);
          } else {
            spdlog::debug("validation layer {}: {}", flag_to_string(flags), p_callback_data->pMessage);
          }
          return VK_FALSE;
        },
    };
    VK_CALL(vkCreateDebugUtilsMessengerEXT(vulkan_instance_, &create_info, nullptr, &debug_messenger_));
  }
}

void application::Application::PickPhysicalDevice() {
  uint32_t device_count = 0;
  VK_CALL(vkEnumeratePhysicalDevices(vulkan_instance_, &device_count, nullptr));
  if (device_count == 0) {
    throw std::runtime_error("failed to find GPUs with Vulkan support!");
  }
  std::vector<VkPhysicalDevice> devices(device_count);
  VK_CALL(vkEnumeratePhysicalDevices(vulkan_instance_, &device_count, devices.data()));
  for (const auto &kDevice : devices) {
    auto available_extensions = vulkan::GetDeviceExtensions(kDevice);
    std::set<std::string> required_extensions(required_device_extentions.begin(), required_device_extentions.end());
    for (const auto &kExtension : available_extensions) {
      required_extensions.erase(kExtension.extensionName);
    }
    bool extensions_supported = required_extensions.empty();
    bool has_synch_2_ext = std::find_if(available_extensions.begin(), available_extensions.end(), [](auto it) {
      return strcmp(it.extensionName, VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME) == 0;
    }) != available_extensions.end();
    bool swap_chain_adequate = false;
    if (extensions_supported) {
      SwapChainSupportDetails swap_chain_support(kDevice, surface_);
      swap_chain_adequate = !swap_chain_support.formats.empty() && !swap_chain_support.present_modes.empty();
    }
    QueueFamilyIndices indices(kDevice, surface_);
    VkPhysicalDeviceVulkan13Features suppported_13_features{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
    };
    VkPhysicalDeviceFeatures2 supported_features{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
        .pNext = &suppported_13_features,
    };

    vkGetPhysicalDeviceFeatures2(kDevice, &supported_features);
    if (indices.IsComplete() && extensions_supported && swap_chain_adequate
        && (suppported_13_features.synchronization2 == VK_TRUE || has_synch_2_ext)) {
      physical_device_ = kDevice;
      if (suppported_13_features.synchronization2 == VK_FALSE && has_synch_2_ext) {
        required_device_extentions.emplace_back(VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME);
      }
      break;
    }
  }
  if (physical_device_ == VK_NULL_HANDLE) {
    throw std::runtime_error("failed to find a suitable GPU!");
  }
  vulkan::LogDeviceInfo(physical_device_);
}

void application::Application::CreateLogicalDevice() {
  QueueFamilyIndices indices(physical_device_, surface_);
  std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
  std::set<uint32_t> unique_queue_families = {indices.graphics_family.value(), indices.present_family.value()};
  float queue_priority = 1.0f;
  for (uint32_t queue_family : unique_queue_families) {
    VkDeviceQueueCreateInfo queue_create_info{
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = queue_family,
        .queueCount = 1,
        .pQueuePriorities = &queue_priority,
    };
    queue_create_infos.push_back(queue_create_info);
  }

  auto device_extensions = vulkan::GetDeviceExtensions(physical_device_);
  std::vector<const char *> extensions_to_enable(required_device_extentions.begin(), required_device_extentions.end());
  for (auto ext : device_extensions) {
    if (strcmp(ext.extensionName, "VK_KHR_portability_subset") == 0) {
      extensions_to_enable.emplace_back("VK_KHR_portability_subset");
    }
  }
  bool use_synch2_ext = std::find(required_device_extentions.begin(),
                                  required_device_extentions.end(),
                                  VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME) != required_device_extentions.end();

  VkPhysicalDeviceVulkan13Features features_enable_13{
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
      .synchronization2 = VK_TRUE,
  };
  VkPhysicalDeviceSynchronization2Features synchronization_2_features{
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES,
      .synchronization2 = VK_TRUE,
  };
  VkPhysicalDeviceFeatures2 features_enable{
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
      .pNext = use_synch2_ext ? static_cast<void *>(&synchronization_2_features) : &features_enable_13,
  };
  VkDeviceCreateInfo create_info = {
      .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
      .pNext = &features_enable,
      .queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size()),
      .pQueueCreateInfos = queue_create_infos.data(),
      .enabledExtensionCount = static_cast<uint32_t>(extensions_to_enable.size()),
      .ppEnabledExtensionNames = extensions_to_enable.data(),
  };

  VK_CALL(vkCreateDevice(physical_device_, &create_info, nullptr, &device_));
  volkLoadDevice(device_);
  vkGetDeviceQueue(device_, indices.present_family.value(), 0, &present_queue_);
  present_family_index_ = indices.present_family.value();
  vkGetDeviceQueue(device_, indices.graphics_family.value(), 0, &graphics_queue_);
  graphics_family_index_ = indices.graphics_family.value();
}

void application::Application::CreateSwapChain() {
  SwapChainSupportDetails swap_chain_support(physical_device_, surface_);
  VkSurfaceFormatKHR surface_format = swap_chain_support.ChooseSwapSurfaceFormat(VK_FORMAT_B8G8R8A8_UNORM);

  int window_width = 0;
  int window_height = 0;
  glfwGetFramebufferSize(window_, &window_width, &window_height);
  VkExtent2D swap_chain_extent = {
      .width =  std::max(swap_chain_support.capabilities.minImageExtent.width,
                         std::min(swap_chain_support.capabilities.maxImageExtent.width,
                                  static_cast<uint32_t>(window_width))),
      .height = std::max(swap_chain_support.capabilities.minImageExtent.height,
                         std::min(swap_chain_support.capabilities.maxImageExtent.height,
                                  static_cast<uint32_t>(window_height))),
  };
  auto swap_chain_image_format = surface_format.format;
  uint32_t image_count = swap_chain_support.capabilities.minImageCount + 1;
  if (swap_chain_support.capabilities.maxImageCount > 0
      && image_count > swap_chain_support.capabilities.maxImageCount) {
    image_count = swap_chain_support.capabilities.maxImageCount;
  }
  VkSwapchainCreateInfoKHR create_info = {
      .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
      .surface = surface_,
      .minImageCount = image_count,
      .imageFormat = surface_format.format,
      .imageColorSpace = surface_format.colorSpace,
      .imageExtent = swap_chain_extent,
      .imageArrayLayers = 1,
      .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
      .preTransform = swap_chain_support.capabilities.currentTransform,
      .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
      .presentMode = swap_chain_support.ChooseSwapPresentMode(),
      .clipped = VK_TRUE,
      .oldSwapchain = VK_NULL_HANDLE,
  };

  QueueFamilyIndices indices(physical_device_, surface_);
  std::array<uint32_t, 2> queue_family_indices = {indices.graphics_family.value(), indices.present_family.value()};
  if (indices.graphics_family != indices.present_family) {
    create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    create_info.queueFamilyIndexCount = 2;
    create_info.pQueueFamilyIndices = queue_family_indices.data();
  } else {
    create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    create_info.queueFamilyIndexCount = 0;
    create_info.pQueueFamilyIndices = nullptr;
  }

  VK_CALL(vkCreateSwapchainKHR(device_, &create_info, nullptr, &swap_chain_));

  VK_CALL(vkGetSwapchainImagesKHR(device_, swap_chain_, &image_count, nullptr));
  std::vector<VkImage> images = std::vector<VkImage>(image_count);
  VK_CALL(vkGetSwapchainImagesKHR(device_, swap_chain_, &image_count, images.data()));
  for (const auto &kImage : images) {
    semaphores_.emplace_back(rendering_context_->CreateSemaphore());
    auto image = vulkan::Image::Create(rendering_context_,
                                       swap_chain_extent.width,
                                       swap_chain_extent.height,
                                       swap_chain_image_format,
                                       kImage);
    swap_chain_images_.emplace_back(image);
  }
}

void application::Application::CleanupSwapChain() {
  for (auto *semaphore : semaphores_) {
    rendering_context_->DestroySemaphore(semaphore);
  }
  semaphores_.clear();
  swap_chain_images_.clear();
  vkDestroySwapchainKHR(device_, swap_chain_, nullptr);
  swap_chain_ = VK_NULL_HANDLE;
}

void application::Application::Run() {
  while (glfwWindowShouldClose(window_) == GLFW_FALSE) {
    glfwPollEvents();
    if (swap_chain_ == VK_NULL_HANDLE) {
      return;
    }

    current_frame_index_ = (current_frame_index_ + 1) % swap_chain_images_.size();

    uint32_t image_index = 0;
    auto result = vkAcquireNextImageKHR(device_,
                                        swap_chain_,
                                        UINT64_MAX,
                                        semaphores_[current_frame_index_],
                                        VK_NULL_HANDLE,
                                        &image_index);
    if (result == VK_SUBOPTIMAL_KHR || result == VK_ERROR_OUT_OF_DATE_KHR) {
      continue;
    }
    VK_CALL(result);
    VkSemaphore semaphore = renderable_->Render(swap_chain_images_[image_index],
                                                semaphores_[current_frame_index_]);

    VkPresentInfoKHR present_info = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &semaphore,
        .swapchainCount = 1,
        .pSwapchains = &swap_chain_,
        .pImageIndices = &image_index,
    };

    result = vkQueuePresentKHR(present_queue_, &present_info);
    if (result == VK_SUBOPTIMAL_KHR || result == VK_ERROR_OUT_OF_DATE_KHR) {
      continue;
    }
    VK_CALL(result);
  }
}
void application::Application::RequestExit() {
  glfwSetWindowShouldClose(window_, GLFW_TRUE);
}

application::Application::~Application() {
  rendering_context_->WaitForGpuIdle();
  renderable_ = nullptr;
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  CleanupSwapChain();
  vkDestroySurfaceKHR(vulkan_instance_, surface_, nullptr);
  rendering_context_ = nullptr;
  vkDestroyDevice(device_, nullptr);
  if (debug_messenger_ != VK_NULL_HANDLE) {
    vkDestroyDebugUtilsMessengerEXT(vulkan_instance_, debug_messenger_, nullptr);
  }
  vkDestroyInstance(vulkan_instance_, nullptr);
  glfwDestroyWindow(window_);
  glfwTerminate();
}

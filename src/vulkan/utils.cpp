#include "utils.hpp"

#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include <magic_enum/magic_enum.hpp>
#include <stdexcept>

void vulkan::CheckResult(const VkResult &result, const std::string &file, uint32_t line) {
  if (result != VK_SUCCESS) [[unlikely]] {
    throw std::runtime_error(
        fmt::format("call failed with error {} {}:{}\n", magic_enum::enum_name(result), file, line));
  }
}

std::vector<VkLayerProperties> vulkan::GetAvailableInstanceLayers() {
  uint32_t count = 0;
  VK_CALL(vkEnumerateInstanceLayerProperties(&count, nullptr));
  std::vector<VkLayerProperties> available_layers(count);
  VK_CALL(vkEnumerateInstanceLayerProperties(&count, available_layers.data()));
  return available_layers;
}

std::vector<VkExtensionProperties> vulkan::GetAvailableInstanceExtensions(const std::string &layer_name) {
  const char *p_layer_name = nullptr;
  if (!layer_name.empty()) {
    p_layer_name = layer_name.data();
  }
  uint32_t count = 0;
  VK_CALL(vkEnumerateInstanceExtensionProperties(p_layer_name, &count, nullptr));
  std::vector<VkExtensionProperties> available_extensions(count);
  VK_CALL(vkEnumerateInstanceExtensionProperties(p_layer_name, &count, available_extensions.data()));
  return available_extensions;
}

std::vector<VkExtensionProperties> vulkan::GetDeviceExtensions(const VkPhysicalDevice &device) {
  uint32_t extension_count = 0;
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);
  std::vector<VkExtensionProperties> available_extensions(extension_count);
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, available_extensions.data());
  return available_extensions;
}

void vulkan::LogDeviceInfo(const VkPhysicalDevice &physical_device) {
  VkPhysicalDeviceDriverProperties driver_properties{
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES,
  };
  VkPhysicalDeviceProperties2 properties_2{
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,
      .pNext = &driver_properties,
  };
  vkGetPhysicalDeviceProperties2(physical_device, &properties_2);

  spdlog::debug(
      "\nGPU device: {} (id: {}, type: {}, api version: {}.{}.{})\n"
      "GPU driver: {} (id: {}, version: {}, conformance version:{}.{}.{}.{}, info: {})\n",
      properties_2.properties.deviceName, properties_2.properties.deviceID,
      magic_enum::enum_name(properties_2.properties.deviceType), VK_VERSION_MAJOR(properties_2.properties.apiVersion),
      VK_VERSION_MINOR(properties_2.properties.apiVersion), VK_VERSION_PATCH(properties_2.properties.apiVersion),
      driver_properties.driverName, magic_enum::enum_name(driver_properties.driverID),
      properties_2.properties.driverVersion, driver_properties.conformanceVersion.major,
      driver_properties.conformanceVersion.minor, driver_properties.conformanceVersion.subminor,
      driver_properties.conformanceVersion.patch, driver_properties.driverInfo);
}

uint32_t vulkan::GetDeviceApiVersion(const VkPhysicalDevice &physical_device) {
  VkPhysicalDeviceProperties2 properties_2{
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,
      .pNext = nullptr,
  };
  vkGetPhysicalDeviceProperties2(physical_device, &properties_2);
  return properties_2.properties.apiVersion;
}

VkSampleCountFlagBits vulkan::GetMaxUsableSampleCount(const VkPhysicalDevice &physical_device) {
  VkPhysicalDeviceProperties physical_device_properties;
  vkGetPhysicalDeviceProperties(physical_device, &physical_device_properties);
  const VkSampleCountFlags counts = physical_device_properties.limits.framebufferColorSampleCounts;
  if (static_cast<bool>(counts & VK_SAMPLE_COUNT_64_BIT)) {
    return VK_SAMPLE_COUNT_64_BIT;
  }
  if (static_cast<bool>(counts & VK_SAMPLE_COUNT_32_BIT)) {
    return VK_SAMPLE_COUNT_32_BIT;
  }
  if (static_cast<bool>(counts & VK_SAMPLE_COUNT_16_BIT)) {
    return VK_SAMPLE_COUNT_16_BIT;
  }
  if (static_cast<bool>(counts & VK_SAMPLE_COUNT_8_BIT)) {
    return VK_SAMPLE_COUNT_8_BIT;
  }
  if (static_cast<bool>(counts & VK_SAMPLE_COUNT_4_BIT)) {
    return VK_SAMPLE_COUNT_4_BIT;
  }
  if (static_cast<bool>(counts & VK_SAMPLE_COUNT_2_BIT)) {
    return VK_SAMPLE_COUNT_2_BIT;
  }
  return VK_SAMPLE_COUNT_1_BIT;
}

namespace {
VkFormat FindSupportedFormat(const VkPhysicalDevice &physical_device, const std::vector<VkFormat> &candidates,
                             const VkImageTiling tiling, const VkFormatFeatureFlags features) {
  for (const VkFormat format : candidates) {
    VkFormatProperties props;
    vkGetPhysicalDeviceFormatProperties(physical_device, format, &props);
    if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
      return format;
    }
    if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
      return format;
    }
  }

  throw std::runtime_error("failed to find supported format!");
}
}  // namespace

VkFormat vulkan::GetDepthFormat(const VkPhysicalDevice &physical_device) {
  return FindSupportedFormat(physical_device, {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D16_UNORM}, VK_IMAGE_TILING_OPTIMAL,
                             VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

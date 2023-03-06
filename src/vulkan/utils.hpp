#pragma once

#include "vulkan_include.hpp"

#include <string>
#include <vector>

#define VK_CALL(x)                   \
    vulkan::CheckResult(x, __FILE__, __LINE__)

namespace vulkan {

void CheckResult(VkResult result, const std::string &file, uint32_t line);

std::vector<VkExtensionProperties> GetAvailableInstanceExtensions(std::string layer_name);

std::vector<VkLayerProperties> GetAvailableInstanceLayers();

std::vector<VkExtensionProperties> GetDeviceExtensions(VkPhysicalDevice device);

void LogDeviceInfo(VkPhysicalDevice physical_device);

uint32_t GetDeviceApiVersion(VkPhysicalDevice physical_device);

VkSampleCountFlagBits GetMaxUsableColorSampleCount(VkPhysicalDevice physical_device);

VkFormat GetDepthFormat(VkPhysicalDevice physical_device);
}  // namespace vulkan

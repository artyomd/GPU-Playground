#pragma once

#include <string>
#include <vector>

#include "include/vulkan_include.hpp"

#define VK_CALL(x) vulkan::CheckResult(x, __FILE__, __LINE__)

namespace vulkan {

void CheckResult(const VkResult &result, const std::string &file, uint32_t line);

std::vector<VkExtensionProperties> GetAvailableInstanceExtensions(const std::string &layer_name = "");

std::vector<VkLayerProperties> GetAvailableInstanceLayers();

std::vector<VkExtensionProperties> GetDeviceExtensions(const VkPhysicalDevice &device);

void LogDeviceInfo(const VkPhysicalDevice &physical_device);

uint32_t GetDeviceApiVersion(const VkPhysicalDevice &physical_device);

VkSampleCountFlagBits GetMaxUsableSampleCount(const VkPhysicalDevice &physical_device);

VkFormat GetDepthFormat(const VkPhysicalDevice &physical_device);
}  // namespace vulkan

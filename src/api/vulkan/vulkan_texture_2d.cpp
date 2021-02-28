//
// Created by artyomd on 3/30/20.
//

#include "src/api/vulkan/vulkan_texture_2d.hpp"

#include <cstring>
#include <stdexcept>
#include <utility>

#include "src/api/vulkan/vulkan_utils.hpp"

api::vulkan::VulkanTexture2D::VulkanTexture2D(
    std::shared_ptr<VulkanRenderingContext> context,
    size_t width,
    size_t height,
    PixelFormat pixel_format)
    : Texture2D(width, height, pixel_format),
      context_(std::move(context)), device_(context_->GetDevice()) {

  context_->CreateImage(width_,
                        height_,
                        VK_SAMPLE_COUNT_1_BIT,
                        GetVkFormat(pixel_format_),
                        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                        &image_,
                        &image_memory_);

  context_->CreateImageView(image_, GetVkFormat(pixel_format_), VK_IMAGE_ASPECT_COLOR_BIT, &image_view_);
  image_info_.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  image_info_.imageView = image_view_;
  image_info_.sampler = VK_NULL_HANDLE;
}

void api::vulkan::VulkanTexture2D::Load(const void *data) {
  VkBuffer staging_buffer;
  VkDeviceMemory staging_buffer_memory;
  auto image_size = GetTextureSizeInBytes();
  context_->CreateBuffer(image_size,
                         VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                         &staging_buffer,
                         &staging_buffer_memory);

  void *mapped_data;
  vkMapMemory(device_, staging_buffer_memory, 0, image_size, 0, &mapped_data);
  memcpy(mapped_data, data, static_cast<size_t>(image_size));
  vkUnmapMemory(device_, staging_buffer_memory);

  context_->TransitionImageLayout(image_, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
  context_->CopyBufferToImage(staging_buffer, image_, static_cast<uint32_t>(width_), static_cast<uint32_t>(height_));
  context_->TransitionImageLayout(image_,
                                  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                  VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

  vkDestroyBuffer(device_, staging_buffer, nullptr);
  vkFreeMemory(device_, staging_buffer_memory, nullptr);
}

VkWriteDescriptorSet api::vulkan::VulkanTexture2D::GetWriteDescriptorSetFor(unsigned int,
                                                                            unsigned int binding_point) const {
  if (image_info_.sampler == VK_NULL_HANDLE) {
    throw std::runtime_error("no sampler defined for the texture");
  }
  VkWriteDescriptorSet write_descriptor_set{};
  write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  write_descriptor_set.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  write_descriptor_set.descriptorCount = 1;
  write_descriptor_set.pImageInfo = &image_info_;
  write_descriptor_set.dstBinding = binding_point;
  write_descriptor_set.dstArrayElement = 0;
  return write_descriptor_set;
}

void api::vulkan::VulkanTexture2D::SetSampler(api::Sampler sampler) {
  if (image_info_.sampler != VK_NULL_HANDLE) {
    vkDestroySampler(device_, image_info_.sampler, nullptr);
  }
  VkSamplerCreateInfo sampler_info = {};
  sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  sampler_info.magFilter = GetVkFilter(sampler.mag_filter);
  sampler_info.minFilter = GetVkFilter(sampler.min_filter);
  sampler_info.addressModeU = GetVkAddressMode(sampler.address_mode_u);
  sampler_info.addressModeV = GetVkAddressMode(sampler.address_mode_v);
  sampler_info.addressModeW = GetVkAddressMode(sampler.address_mode_w);
  sampler_info.anisotropyEnable = VK_FALSE;
  sampler_info.maxAnisotropy = 1.0F;
  sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
  sampler_info.unnormalizedCoordinates = VK_FALSE;
  sampler_info.compareEnable = VK_FALSE;
  sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
  sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

  if (vkCreateSampler(device_, &sampler_info, nullptr, &image_info_.sampler) != VK_SUCCESS) {
    throw std::runtime_error("failed to create texture sampler!");
  }
}

api::vulkan::VulkanTexture2D::~VulkanTexture2D() {
  if (image_info_.sampler != VK_NULL_HANDLE) {
    vkDestroySampler(device_, image_info_.sampler, nullptr);
  }
  vkDestroyImageView(device_, image_view_, nullptr);
  vkDestroyImage(device_, image_, nullptr);
  vkFreeMemory(device_, image_memory_, nullptr);
}

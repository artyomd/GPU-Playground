//
// Created by artyomd on 3/30/20.
//

#include <stdexcept>
#include <utility>
#include <vendor/stb_image/stb_image.h>
#include <cstring>
#include "VkTexture2D.h"
api::VkTexture2D::VkTexture2D(VkRenderingContext *context,
                              std::string image_path,
                              int binding_point,
                              api::ShaderType shader_stage)
    : Texture2D(std::move(image_path), binding_point, shader_stage),
      context_(context), device_(context_->GetDevice()) {

  stbi_set_flip_vertically_on_load(1);
  int tex_width, tex_height, tex_channels;
  stbi_uc *pixels = stbi_load(image_path_.c_str(), &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);
  if (!pixels) {
    throw std::runtime_error("failed to load texture image!");
  }

  VkDeviceSize image_size = tex_width*tex_height*4;
  VkBuffer staging_buffer;
  VkDeviceMemory staging_buffer_memory;
  context->CreateBuffer(image_size,
                        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                        staging_buffer,
                        staging_buffer_memory);

  void *data;
  vkMapMemory(*device_, staging_buffer_memory, 0, image_size, 0, &data);
  memcpy(data, pixels, static_cast<size_t>(image_size));
  vkUnmapMemory(*device_, staging_buffer_memory);

  stbi_image_free(pixels);

  CreateImage(tex_width,
              tex_height,
              VK_FORMAT_R8G8B8A8_SRGB,
              VK_IMAGE_TILING_OPTIMAL,
              VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
              image_,
              image_memory_);

  context_->TransitionImageLayout(image_,
                                  VK_IMAGE_LAYOUT_UNDEFINED,
                                  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
  context_->CopyBufferToImage(staging_buffer,
                              image_,
                              static_cast<uint32_t>(tex_width),
                              static_cast<uint32_t>(tex_height));
  context_->TransitionImageLayout(image_,
                                  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                  VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

  vkDestroyBuffer(*device_, staging_buffer, nullptr);
  vkFreeMemory(*device_, staging_buffer_memory, nullptr);

  image_view_ = context_->CreateImageView(image_, VK_FORMAT_R8G8B8A8_SRGB);
  CreateTextureSampler();
  image_info_.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  image_info_.imageView = image_view_;
  image_info_.sampler = sampler_;

}

void api::VkTexture2D::CreateTextureSampler() {
  VkSamplerCreateInfo sampler_info = {};
  sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  sampler_info.magFilter = VK_FILTER_LINEAR;
  sampler_info.minFilter = VK_FILTER_LINEAR;
  sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  sampler_info.anisotropyEnable = VK_FALSE;
  sampler_info.maxAnisotropy = 1;
  sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
  sampler_info.unnormalizedCoordinates = VK_FALSE;
  sampler_info.compareEnable = VK_FALSE;
  sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
  sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

  if (vkCreateSampler(*device_, &sampler_info, nullptr, &sampler_)!=VK_SUCCESS) {
    throw std::runtime_error("failed to create texture sampler!");
  }
}

void api::VkTexture2D::CreateImage(uint32_t width,
                                   uint32_t height,
                                   VkFormat format,
                                   VkImageTiling tiling,
                                   VkImageUsageFlags usage,
                                   VkMemoryPropertyFlags properties,
                                   VkImage &image,
                                   VkDeviceMemory &image_memory) {
  VkImageCreateInfo image_info = {};
  image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  image_info.imageType = VK_IMAGE_TYPE_2D;
  image_info.extent.width = width;
  image_info.extent.height = height;
  image_info.extent.depth = 1;
  image_info.mipLevels = 1;
  image_info.arrayLayers = 1;
  image_info.format = format;
  image_info.tiling = tiling;
  image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  image_info.usage = usage;
  image_info.samples = VK_SAMPLE_COUNT_1_BIT;
  image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateImage(*device_, &image_info, nullptr, &image)!=VK_SUCCESS) {
    throw std::runtime_error("failed to create image!");
  }

  VkMemoryRequirements mem_requirements;
  vkGetImageMemoryRequirements(*device_, image, &mem_requirements);

  VkMemoryAllocateInfo alloc_info = {};
  alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  alloc_info.allocationSize = mem_requirements.size;
  alloc_info.memoryTypeIndex = context_->FindMemoryType(mem_requirements.memoryTypeBits, properties);

  if (vkAllocateMemory((*device_), &alloc_info, nullptr, &image_memory)!=VK_SUCCESS) {
    throw std::runtime_error("failed to allocate image memory!");
  }

  vkBindImageMemory(*device_, image, image_memory, 0);
}

VkDescriptorSetLayoutBinding api::VkTexture2D::GetLayoutBinding() const {
  VkDescriptorSetLayoutBinding layout_binding = {};
  layout_binding.binding = 1;
  layout_binding.descriptorCount = 1;
  layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  layout_binding.pImmutableSamplers = nullptr;
  layout_binding.stageFlags = GetVkShaderStageFlag(shader_stage_);
  return layout_binding;
}

VkWriteDescriptorSet api::VkTexture2D::GetWriteDescriptorSetFor(int image_index) const {
  VkWriteDescriptorSet descriptor_writes = {};
  descriptor_writes.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  descriptor_writes.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  descriptor_writes.descriptorCount = 1;
  descriptor_writes.pImageInfo = &image_info_;
  return descriptor_writes;
}

api::VkTexture2D::~VkTexture2D() {
  vkDestroySampler(*device_, sampler_, nullptr);
  vkDestroyImageView(*device_, image_view_, nullptr);
  vkDestroyImage(*device_, image_, nullptr);
  vkFreeMemory(*device_, image_memory_, nullptr);
}


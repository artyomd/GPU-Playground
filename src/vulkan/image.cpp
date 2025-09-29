#include "image.hpp"

#include <map>
#include <stdexcept>

#include "utils.hpp"

std::shared_ptr<vulkan::Image> vulkan::Image::Create(const std::shared_ptr<RenderingContext> &context,
                                                     const uint32_t width, const uint32_t height,
                                                     const VkFormat pixel_format, const VkImageUsageFlags usage,
                                                     const bool host_visible,
                                                     const VkSampleCountFlagBits sample_count) {
  return std::shared_ptr<Image>(new Image(context, width, height, pixel_format, usage, host_visible, sample_count));
}

std::shared_ptr<vulkan::Image> vulkan::Image::Create(const std::shared_ptr<RenderingContext> &context,
                                                     const uint32_t width, const uint32_t height,
                                                     const VkFormat pixel_format, const VkImage &image) {
  return std::shared_ptr<Image>(new Image(context, width, height, pixel_format, image));
}

vulkan::Image::Image(const std::shared_ptr<RenderingContext> &context, const uint32_t width, const unsigned int height,
                     const VkFormat pixel_format, const VkImageUsageFlags usage, const bool host_visible,
                     const VkSampleCountFlagBits sample_count)
    : context_(context), width_(width), height_(height), pixel_format_(pixel_format), host_visible_(host_visible) {
  const VkImageCreateInfo image_info = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
      .imageType = VK_IMAGE_TYPE_2D,
      .format = pixel_format_,
      .extent =
          {
              .width = width,
              .height = height,
              .depth = 1,
          },
      .mipLevels = 1,
      .arrayLayers = 1,
      .samples = sample_count,
      .tiling = host_visible_ ? VK_IMAGE_TILING_LINEAR : VK_IMAGE_TILING_OPTIMAL,
      .usage = usage,
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
      .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
  };

  const VmaAllocationCreateInfo alloc_info = {
      .flags = static_cast<VmaAllocationCreateFlags>(
          host_visible_ ? VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT : 0),
      .usage = host_visible_ ? VMA_MEMORY_USAGE_AUTO_PREFER_HOST : VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
  };

  VK_CALL(vmaCreateImage(context_->GetAllocator(), &image_info, &alloc_info, &image_, &allocation_, nullptr));
}

vulkan::Image::Image(const std::shared_ptr<RenderingContext> &context, const uint32_t width, const uint32_t height,
                     const VkFormat pixel_format, const VkImage &image)
    : context_(context), width_(width), height_(height), pixel_format_(pixel_format), image_(image) {}

void vulkan::Image::BlitFrom(const VkCommandBuffer &command_buffer, const std::shared_ptr<Image> &other) {
  TransferTo(command_buffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
  other->TransferTo(command_buffer, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

  const VkImageBlit copy_region = {
      .srcSubresource =
          {
              .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
              .mipLevel = 0,
              .baseArrayLayer = 0,
              .layerCount = 1,
          },
      .srcOffsets = {{0, 0, 0}, {static_cast<int32_t>(other->width_), static_cast<int32_t>(other->height_), 1}},
      .dstSubresource =
          {
              .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
              .mipLevel = 0,
              .baseArrayLayer = 0,
              .layerCount = 1,
          },
      .dstOffsets = {{0, 0, 0}, {static_cast<int32_t>(width_), static_cast<int32_t>(height_), 1}},
  };
  vkCmdBlitImage(command_buffer, other->image_, other->current_layout_, image_, current_layout_, 1, &copy_region,
                 VK_FILTER_LINEAR);
}

void vulkan::Image::CopyFrom(const VkCommandBuffer &command_buffer, const std::shared_ptr<Image> &other) {
  TransferTo(command_buffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
  other->TransferTo(command_buffer, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

  const VkImageCopy region = {
      .srcSubresource =
          {
              .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
              .mipLevel = 0,
              .baseArrayLayer = 0,
              .layerCount = 1,
          },
      .srcOffset = {0, 0, 0},
      .dstSubresource =
          {
              .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
              .mipLevel = 0,
              .baseArrayLayer = 0,
              .layerCount = 1,
          },
      .dstOffset = {0, 0, 0},
      .extent = {width_, height_, 1},
  };

  vkCmdCopyImage(command_buffer, other->image_, other->current_layout_, image_, current_layout_, 1, &region);
}

void vulkan::Image::CopyFrom(const VkCommandBuffer &command_buffer, const std::shared_ptr<Buffer> &other) {
  TransferTo(command_buffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
  const VkBufferImageCopy region = {
      .bufferOffset = 0,
      .bufferRowLength = 0,
      .bufferImageHeight = 0,
      .imageSubresource =
          {
              .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
              .mipLevel = 0,
              .baseArrayLayer = 0,
              .layerCount = 1,
          },
      .imageOffset = {0, 0, 0},
      .imageExtent = {width_, height_, 1},
  };

  vkCmdCopyBufferToImage(command_buffer, other->GetBuffer(), image_, current_layout_, 1, &region);
}

void vulkan::Image::CopyTo(const VkCommandBuffer &command_buffer, const std::shared_ptr<Buffer> &other) {
  TransferTo(command_buffer, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
  const VkBufferImageCopy region = {
      .bufferOffset = 0,
      .bufferRowLength = 0,
      .bufferImageHeight = 0,
      .imageSubresource =
          {
              .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
              .mipLevel = 0,
              .baseArrayLayer = 0,
              .layerCount = 1,
          },
      .imageOffset = {0, 0, 0},
      .imageExtent = {width_, height_, 1},
  };

  vkCmdCopyImageToBuffer(command_buffer, image_, current_layout_, other->GetBuffer(), 1, &region);
}

void *vulkan::Image::Map() const {
  if (!host_visible_) {
    throw std::runtime_error("only supported on host visible textures");
  }
  void *mapped_data = nullptr;
  VK_CALL(vmaMapMemory(context_->GetAllocator(), allocation_, &mapped_data));
  return mapped_data;
}

void vulkan::Image::Unmap() const {
  if (!host_visible_) {
    throw std::runtime_error("only supported on host visible textures");
  }
  vmaUnmapMemory(context_->GetAllocator(), allocation_);
}

void vulkan::Image::TransferTo(const VkCommandBuffer &command_buffer, const VkImageLayout new_layout) {
  if (new_layout == current_layout_) {
    return;
  }
  VkImageAspectFlags aspect_flags = VK_IMAGE_ASPECT_COLOR_BIT;
  if (pixel_format_ == VK_FORMAT_D16_UNORM || pixel_format_ == VK_FORMAT_D32_SFLOAT) {
    aspect_flags = VK_IMAGE_ASPECT_DEPTH_BIT;
  }
  const std::map<VkImageLayout, std::pair<VkAccessFlags2, VkPipelineStageFlagBits2>> layout_mapping = {
      std::make_pair(VK_IMAGE_LAYOUT_UNDEFINED, std::make_pair(0, VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT)),
      std::make_pair(VK_IMAGE_LAYOUT_GENERAL, std::make_pair(0, VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT)),
      std::make_pair(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                     std::make_pair(VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT,
                                    VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT)),
      std::make_pair(
          VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
          std::make_pair(VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                         VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT)),
      std::make_pair(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                     std::make_pair(VK_ACCESS_2_SHADER_READ_BIT, VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT)),
      std::make_pair(VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                     std::make_pair(VK_ACCESS_2_TRANSFER_READ_BIT, VK_PIPELINE_STAGE_2_COPY_BIT)),
      std::make_pair(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                     std::make_pair(VK_ACCESS_2_TRANSFER_WRITE_BIT, VK_PIPELINE_STAGE_2_COPY_BIT)),

  };
  const VkImageMemoryBarrier2 barrier = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
      .srcStageMask = layout_mapping.at(current_layout_).second,
      .srcAccessMask = layout_mapping.at(current_layout_).first,
      .dstStageMask = layout_mapping.at(new_layout).second,
      .dstAccessMask = layout_mapping.at(new_layout).first,
      .oldLayout = current_layout_,
      .newLayout = new_layout,
      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .image = image_,
      .subresourceRange =
          {
              .aspectMask = aspect_flags,
              .baseMipLevel = 0,
              .levelCount = 1,
              .baseArrayLayer = 0,
              .layerCount = 1,
          },
  };
  const VkDependencyInfo dependency_info{
      .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
      .pNext = nullptr,
      .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
      .imageMemoryBarrierCount = 1,
      .pImageMemoryBarriers = &barrier,
  };
  if (context_->IsUseSynch2Ext()) {
    vkCmdPipelineBarrier2KHR(command_buffer, &dependency_info);
  } else {
    vkCmdPipelineBarrier2(command_buffer, &dependency_info);
  }
  current_layout_ = new_layout;
}

VkImage vulkan::Image::GetImage() const { return image_; }

vulkan::Image::~Image() {
  if (allocation_ != nullptr) {
    vmaDestroyImage(context_->GetAllocator(), image_, allocation_);
  }
}
namespace {
size_t GetPixelFormatSize(const VkFormat format) {
  switch (format) {
    case VK_FORMAT_R8G8B8A8_UNORM:
    case VK_FORMAT_R8G8B8A8_SRGB:
      return 4;
    default:
      throw std::runtime_error("unimplemented");
  }
}
}  // namespace
size_t vulkan::Image::GetSizeInBytes() const {
  return static_cast<size_t>(width_ * height_) * GetPixelFormatSize(pixel_format_);
}

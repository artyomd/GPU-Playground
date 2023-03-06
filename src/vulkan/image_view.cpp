#include "image_view.hpp"

#include "utils.hpp"

std::shared_ptr<vulkan::ImageView> vulkan::ImageView::Create(std::shared_ptr<RenderingContext> rendering_context,
                                                             std::shared_ptr<Image> image) {
  auto image_view = new ImageView(rendering_context, image);
  return std::shared_ptr<ImageView>(image_view);
}

vulkan::ImageView::ImageView(std::shared_ptr<RenderingContext> rendering_context, std::shared_ptr<Image> image) :
    rendering_context_(rendering_context), image_(image), aspect_flags_(VK_IMAGE_ASPECT_COLOR_BIT) {
  if (image_->GetPixelFormat() == VK_FORMAT_D16_UNORM || image_->GetPixelFormat() == VK_FORMAT_D32_SFLOAT) {
    aspect_flags_ = VK_IMAGE_ASPECT_DEPTH_BIT;
  }
  VkImageViewCreateInfo image_view_create_info{
      .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .image = image_->GetImage(),
      .viewType = VK_IMAGE_VIEW_TYPE_2D,
      .format = image_->GetPixelFormat(),
      .components = {
          .r = VK_COMPONENT_SWIZZLE_R,
          .g = VK_COMPONENT_SWIZZLE_G,
          .b = VK_COMPONENT_SWIZZLE_B,
          .a = VK_COMPONENT_SWIZZLE_A,
      },
      .subresourceRange= {
          .aspectMask = aspect_flags_,
          .baseMipLevel = 0,
          .levelCount = 1,
          .baseArrayLayer = 0,
          .layerCount = 1,
      },
  };
  VK_CALL(vkCreateImageView(rendering_context_->GetDevice(), &image_view_create_info, nullptr, &image_view_));
}

VkImageView vulkan::ImageView::GetImageView() const {
  return image_view_;
}

uint32_t vulkan::ImageView::GetWidth() const {
  return image_->GetWidth();
}
uint32_t vulkan::ImageView::GetHeight() const {
  return image_->GetHeight();
}

vulkan::ImageView::~ImageView() {
  vkDestroyImageView(rendering_context_->GetDevice(), image_view_, nullptr);
}
VkImageAspectFlags vulkan::ImageView::GetAspectMask() {
  return aspect_flags_;
}
const std::shared_ptr<vulkan::Image> &vulkan::ImageView::GetImage() const {
  return image_;
}

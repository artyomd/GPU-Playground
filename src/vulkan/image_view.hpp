#pragma once

#include "rendering_context.hpp"
#include "image.hpp"

namespace vulkan {
class ImageView {
 public:
  static std::shared_ptr<ImageView> Create(std::shared_ptr<RenderingContext> rendering_context,
                                           std::shared_ptr<Image> image);

  ImageView() = delete;
  ImageView(const ImageView &) = delete;

  [[nodiscard]] VkImageAspectFlags GetAspectMask();
  [[nodiscard]] VkImageView GetImageView() const;
  [[nodiscard]] uint32_t GetWidth() const;
  [[nodiscard]] uint32_t GetHeight() const;
  [[nodiscard]] const std::shared_ptr<Image> &GetImage() const;

  virtual ~ImageView();
 private:
  ImageView(std::shared_ptr<RenderingContext> rendering_context, std::shared_ptr<Image> image);

  VkImageView image_view_ = VK_NULL_HANDLE;
  std::shared_ptr<RenderingContext> rendering_context_ = nullptr;

  std::shared_ptr<Image> image_ = nullptr;
  VkImageAspectFlags aspect_flags_ = VK_IMAGE_ASPECT_NONE;
};
}  // namespace vulkan
#pragma once

#include "image.hpp"
#include "rendering_context.hpp"

namespace vulkan {
class ImageView final {
 public:
  static std::shared_ptr<ImageView> Create(const std::shared_ptr<RenderingContext>& rendering_context,
                                           const std::shared_ptr<Image>& image);

  ImageView() = delete;
  ImageView(const ImageView&) = delete;
  ImageView(ImageView&&) = delete;

  ImageView& operator=(const ImageView&) = delete;
  ImageView& operator=(ImageView&&) = delete;

  [[nodiscard]] VkImageAspectFlags GetAspectMask() const;
  [[nodiscard]] VkImageView GetImageView() const;
  [[nodiscard]] uint32_t GetWidth() const;
  [[nodiscard]] uint32_t GetHeight() const;
  [[nodiscard]] const std::shared_ptr<Image>& GetImage() const;

  ~ImageView();

 private:
  ImageView(const std::shared_ptr<RenderingContext>& rendering_context, const std::shared_ptr<Image>& image);

  VkImageView image_view_ = VK_NULL_HANDLE;
  std::shared_ptr<RenderingContext> rendering_context_ = nullptr;

  std::shared_ptr<Image> image_ = nullptr;
  VkImageAspectFlags aspect_flags_ = VK_IMAGE_ASPECT_NONE;
};
}  // namespace vulkan

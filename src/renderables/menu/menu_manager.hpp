#pragma once

#include "application/renderable.hpp"
#include "renderables/imgui_wrapper.hpp"
#include "menu.hpp"
#include "vulkan/framebuffer.hpp"
#include "vulkan/render_pass.hpp"
#include "vulkan/rendering_context.hpp"

#include <map>

namespace renderable {
class MenuManager
    : public application::Renderable, public renderable::Menu, public std::enable_shared_from_this<MenuManager> {
 private:
  std::shared_ptr<vulkan::RenderingContext> rendering_context_ = nullptr;
  renderable::MenuItemCreator default_renderable_creator_;

  std::function<void()> exit_function_;
  std::weak_ptr<Menu> parent_;

  std::vector<std::shared_ptr<vulkan::Image>> latest_images_{};
  std::map<std::string, renderable::MenuItemCreator> items_{};
  renderable::MenuItemCreator next_item_creator_ = nullptr;
  std::shared_ptr<Renderable> current_item_ = nullptr;

  MenuManager(std::shared_ptr<vulkan::RenderingContext> context,
              renderable::MenuItemCreator display_creator,
              std::function<void()> exit_function = nullptr,
              std::shared_ptr<Menu> parent = nullptr);
 public:
  static std::shared_ptr<MenuManager> Create(std::shared_ptr<vulkan::RenderingContext> context,
                                             renderable::MenuItemCreator display_creator,
                                             std::shared_ptr<Menu> parent = nullptr);
  static std::shared_ptr<MenuManager> Create(std::shared_ptr<vulkan::RenderingContext> context,
                                             renderable::MenuItemCreator display_creator,
                                             std::function<void()> exit_function = nullptr);
  MenuManager() = delete;
  MenuManager(const MenuManager &) = delete;
  void RegisterMenuItem(std::function<std::shared_ptr<application::Renderable>(std::shared_ptr<vulkan::RenderingContext>,
                                                                               std::shared_ptr<Menu>)> menu_item_creator,
                        std::string display_name) override;
  void PopSelf() override;
  void Select(std::string display_name) override;

  std::set<std::string> EntryNames() override;
  void Pop() override;
  void SetupImages(std::vector<std::shared_ptr<vulkan::Image>> images) override;
  VkSemaphore Render(std::shared_ptr<vulkan::Image> image, const VkSemaphore &semaphore) override;
  ~MenuManager() override = default;
};
} // namespace renderable
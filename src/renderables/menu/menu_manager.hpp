#pragma once

#include <map>

#include "application/renderable.hpp"
#include "menu.hpp"
#include "vulkan/rendering_context.hpp"

namespace renderable {
class MenuManager final : public application::Renderable,
                          public Menu,
                          public std::enable_shared_from_this<MenuManager> {
  std::shared_ptr<vulkan::RenderingContext> rendering_context_ = nullptr;
  MenuItemCreator default_renderable_creator_;

  std::function<void()> exit_function_;
  std::weak_ptr<Menu> parent_;

  std::vector<std::shared_ptr<vulkan::Image>> latest_images_;
  std::map<std::string, MenuItemCreator> items_;
  MenuItemCreator next_item_creator_ = nullptr;
  std::shared_ptr<Renderable> current_item_ = nullptr;

  MenuManager(const std::shared_ptr<vulkan::RenderingContext> &context, MenuItemCreator display_creator,
              const std::function<void()> &exit_function = nullptr, const std::shared_ptr<Menu> &parent = nullptr);

 public:
  static std::shared_ptr<MenuManager> Create(const std::shared_ptr<vulkan::RenderingContext> &context,
                                             const MenuItemCreator &display_creator,
                                             const std::shared_ptr<Menu> &parent = nullptr);
  static std::shared_ptr<MenuManager> Create(const std::shared_ptr<vulkan::RenderingContext> &context,
                                             const MenuItemCreator &display_creator,
                                             const std::function<void()> &exit_function = nullptr);
  MenuManager() = delete;
  MenuManager(const MenuManager &) = delete;
  MenuManager(MenuManager &&) = delete;
  MenuManager &operator=(const MenuManager &) = delete;
  MenuManager &operator=(MenuManager &&) = delete;

  void RegisterMenuItem(const MenuItemCreator &menu_item_creator, const std::string &display_name) override;
  void PopSelf() override;
  void Select(const std::string &display_name) override;

  std::set<std::string> EntryNames() override;
  void Pop() override;
  void SetupImages(const std::vector<std::shared_ptr<vulkan::Image>> &images) override;
  VkSemaphore Render(const std::shared_ptr<vulkan::Image> &image, const VkSemaphore &semaphore) override;
  ~MenuManager() override = default;
};
}  // namespace renderable

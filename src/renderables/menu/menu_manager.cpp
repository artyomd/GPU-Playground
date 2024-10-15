#include "menu_manager.hpp"

#include <iterator>
#include <utility>

std::shared_ptr<renderable::MenuManager> renderable::MenuManager::Create(
    const std::shared_ptr<vulkan::RenderingContext> &context, const MenuItemCreator &display_creator,
    const std::shared_ptr<Menu> &parent) {
  auto *menu = new MenuManager(context, display_creator, nullptr, parent);
  return std::shared_ptr<MenuManager>(menu);
}

std::shared_ptr<renderable::MenuManager> renderable::MenuManager::Create(
    const std::shared_ptr<vulkan::RenderingContext> &context, const MenuItemCreator &display_creator,
    const std::function<void()> &exit_function) {
  auto *menu = new MenuManager(context, display_creator, exit_function, nullptr);
  return std::shared_ptr<MenuManager>(menu);
}

renderable::MenuManager::MenuManager(const std::shared_ptr<vulkan::RenderingContext> &context,
                                     MenuItemCreator display_creator, const std::function<void()> &exit_function,
                                     const std::shared_ptr<Menu> &parent)
    : rendering_context_(context),
      default_renderable_creator_(std::move(display_creator)),
      exit_function_(exit_function),
      parent_(parent),
      next_item_creator_(default_renderable_creator_) {}

void renderable::MenuManager::RegisterMenuItem(const MenuItemCreator &menu_item_creator,
                                               const std::string &display_name) {
  items_[display_name] = menu_item_creator;
}

void renderable::MenuManager::SetupImages(const std::vector<std::shared_ptr<vulkan::Image>> &images) {
  latest_images_ = images;
  if (current_item_ != nullptr) {
    current_item_->SetupImages(images);
  }
}

void renderable::MenuManager::Pop() { next_item_creator_ = default_renderable_creator_; }

std::set<std::string> renderable::MenuManager::EntryNames() {
  std::set<std::string> key_set;
  std::ranges::transform(items_, std::inserter(key_set, key_set.end()), [](auto pair) { return pair.first; });
  return key_set;
}

void renderable::MenuManager::Select(const std::string &display_name) {
  if (items_.contains(display_name)) {
    next_item_creator_ = items_[display_name];
  }
}

void renderable::MenuManager::PopSelf() {
  const auto locked_parent = parent_.lock();
  if (locked_parent != nullptr) {
    locked_parent->Pop();
  } else if (exit_function_ != nullptr) {
    exit_function_();
  }
}

void renderable::MenuManager::Render(const std::shared_ptr<vulkan::Image> &image, const VkSemaphore &waitSemaphore,
                                     const VkSemaphore &signalSemaphore) {
  if (next_item_creator_ != nullptr) {
    current_item_ = next_item_creator_(rendering_context_, shared_from_this());
    current_item_->SetupImages(latest_images_);
    next_item_creator_ = nullptr;
  }
  current_item_->Render(image, waitSemaphore, signalSemaphore);
}


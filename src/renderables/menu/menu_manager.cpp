#include "menu_manager.hpp"

std::shared_ptr<renderable::MenuManager> renderable::MenuManager::Create(std::shared_ptr<vulkan::RenderingContext> context,
                                                                         renderable::MenuItemCreator display_creator,
                                                                         std::shared_ptr<Menu> parent) {
  auto menu = new MenuManager(context, display_creator, nullptr, parent);
  return std::shared_ptr<MenuManager>(menu);
}
std::shared_ptr<renderable::MenuManager> renderable::MenuManager::Create(std::shared_ptr<vulkan::RenderingContext> context,
                                                                         renderable::MenuItemCreator display_creator,
                                                                         std::function<void()> exit_function) {
  auto menu = new MenuManager(context, display_creator, exit_function, nullptr);
  return std::shared_ptr<MenuManager>(menu);
}

renderable::MenuManager::MenuManager(std::shared_ptr<vulkan::RenderingContext> context,
                                     renderable::MenuItemCreator display_creator,
                                     std::function<void()> exit_function,
                                     std::shared_ptr<Menu> parent)
    : rendering_context_(context),
      default_renderable_creator_(display_creator),
      exit_function_(exit_function),
      parent_(parent),
      next_item_creator_(default_renderable_creator_) {

}
void renderable::MenuManager::RegisterMenuItem(std::function<std::shared_ptr<application::Renderable>(std::shared_ptr<
    vulkan::RenderingContext>, std::shared_ptr<Menu>)> menu_item_creator, std::string display_name) {
  items_[display_name] = menu_item_creator;
}
void renderable::MenuManager::SetupImages(std::vector<std::shared_ptr<vulkan::Image>> images) {
  latest_images_ = images;
  if (current_item_ != nullptr) {
    current_item_->SetupImages(images);
  }
}

void renderable::MenuManager::Pop() {
  next_item_creator_ = default_renderable_creator_;
}

std::set<std::string> renderable::MenuManager::EntryNames() {
  std::set<std::string> key_set;
  std::transform(items_.begin(),
                 items_.end(),
                 std::inserter(key_set, key_set.end()),
                 [](auto pair) { return pair.first; });
  return key_set;
}

void renderable::MenuManager::Select(std::string display_name) {
  if (items_.find(display_name) != items_.end() /*c++20 contains*/) {
    next_item_creator_ = items_[display_name];
  }
}

void renderable::MenuManager::PopSelf() {
  auto locked_parent = parent_.lock();
  if (locked_parent != nullptr) {
    locked_parent->Pop();
  } else if (exit_function_ != nullptr) {
    exit_function_();
  }
}

VkSemaphore renderable::MenuManager::Render(std::shared_ptr<vulkan::Image> image, VkSemaphore const &semaphore) {
  if (next_item_creator_ != nullptr) {
    current_item_ = next_item_creator_(rendering_context_, shared_from_this());
    current_item_->SetupImages(latest_images_);
    next_item_creator_ = nullptr;
  }
  return current_item_->Render(image, semaphore);
}



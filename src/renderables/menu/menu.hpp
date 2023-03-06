#pragma once

#include "application/renderable.hpp"

#include <functional>
#include <memory>
#include <set>

namespace renderable {
class Menu;
typedef std::function<std::shared_ptr<application::Renderable>(std::shared_ptr<vulkan::RenderingContext>,
                                                               std::shared_ptr<Menu> parent)> MenuItemCreator;
class Menu {
 public:
  virtual void RegisterMenuItem(MenuItemCreator menu_item_creator, std::string display_name) = 0;
  virtual void Pop() = 0;
  virtual void PopSelf() = 0;
  virtual std::set<std::string> EntryNames() = 0;
  virtual void Select(std::string display_name) = 0;
  virtual ~Menu() = default;
};
} // namespace renderable
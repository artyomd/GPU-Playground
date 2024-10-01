#pragma once

#include <functional>
#include <set>

#include "application/renderable.hpp"

namespace renderable {
class Menu;
using MenuItemCreator = std::function<std::shared_ptr<application::Renderable>(
    const std::shared_ptr<vulkan::RenderingContext>&, const std::shared_ptr<Menu>& parent)>;
class Menu {
 public:
  Menu() = default;
  Menu(const Menu&) = delete;
  Menu(Menu&&) = delete;
  Menu& operator=(const Menu&) = delete;
  Menu& operator=(Menu&&) = delete;
  virtual void RegisterMenuItem(const MenuItemCreator& menu_item_creator, const std::string& display_name) = 0;
  virtual void Pop() = 0;
  virtual void PopSelf() = 0;
  virtual std::set<std::string> EntryNames() = 0;
  virtual void Select(const std::string& display_name) = 0;
  virtual ~Menu() = default;
};
}  // namespace renderable

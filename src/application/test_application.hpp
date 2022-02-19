#pragma once

#include "src/application/application.hpp"
#include "src/test/test.h"
#include "src/test/test_menu.hpp"

namespace application {
class TestApplication : public Application {
 private:
  std::shared_ptr<api::RenderingContext> context_ = nullptr;
  std::shared_ptr<test::Test> current_test_ = nullptr;
  std::shared_ptr<test::TestMenu> test_menu_ = nullptr;

 protected:
  TestApplication();
  virtual void OnTestChanged(std::shared_ptr<test::Test> current_test) = 0;
  [[nodiscard]] std::shared_ptr<api::RenderingContext> GetContext() const;
  void SetContext(const std::shared_ptr<api::RenderingContext> &context);
  [[nodiscard]] const std::shared_ptr<test::Test> &GetCurrentTest() const;
  void InitTestMenu();
  void RenderMenu();
  void CleanupTestMenu();

 public:
  template<typename T>
  void RegisterTest(const std::string &name) {
    if (test_menu_ == nullptr) {
      InitTestMenu();
    }
    test_menu_->RegisterTest<T>(name);
  }
};
}  // namespace application

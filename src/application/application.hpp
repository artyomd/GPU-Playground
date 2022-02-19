#pragma once

#include <memory>
#include <utility>

#include "src/api/rendering_context.hpp"

namespace application {
class Application {
 protected:
  Application() = default;
  virtual void InitContext() = 0;
  virtual void Loop() = 0;
  virtual void DestroyContext() = 0;

 public:
  virtual void Run() final {
    InitContext();
    Loop();
    DestroyContext();
  }

  virtual ~Application() = default;
};
}  // namespace application

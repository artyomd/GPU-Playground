#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include <string>

#include "src/utils/compiler.hpp"

#define LOG_FATAL(...)                                     \
  spdlog::critical("[FATAL] {}:{} {}", __FILE__, __LINE__, \
                   fmt::format(__VA_ARGS__));              \
  std::abort();
#define CHECK(condition, ...)                                                 \
  if (UTILS_UNLIKELY(!(condition))) {                                         \
    LOG_FATAL("Check failed: `" #condition "` {}", fmt::format(__VA_ARGS__)); \
  }

#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include <string>

#include "src/utils/compiler.hpp"

#define LOG_FATAL(args...)                                                     \
  spdlog::critical("[FATAL] {}:{} {}", __FILE__, __LINE__, fmt::format(args)); \
  std::abort();
#define CHECK(condition, args...)                                      \
  if (UTILS_UNLIKELY(condition == false)) {                            \
    LOG_FATAL("Check failed: `" #condition "` {}", fmt::format(args)); \
  }

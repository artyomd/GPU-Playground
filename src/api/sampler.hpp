#pragma once

#include "src/api/data_type.hpp"

namespace api {
enum class Filter {
  LINEAR,
  NEAREST,
  COUNT,
};

enum class AddressMode {
  REPEAT = 0,
  MIRRORED_REPEAT = 1,
  CLAMP_TO_EDGE = 2,
  CLAMP_TO_BORDER = 3,
  COUNT,
};

struct Sampler {
  Filter mag_filter = Filter::LINEAR;
  Filter min_filter = Filter::NEAREST;
  AddressMode address_mode_u = AddressMode::REPEAT;
  AddressMode address_mode_v = AddressMode::REPEAT;
  AddressMode address_mode_w = AddressMode::REPEAT;
};
}  // namespace api

//
// Created by Artyom Dangizyan on 10/8/18.
//
#pragma once

#include <utility>

#include "src/api/buffer.hpp"

namespace api {
class VertexBuffer : public virtual Buffer {
 protected:
  VertexBuffer() = default;
};
}

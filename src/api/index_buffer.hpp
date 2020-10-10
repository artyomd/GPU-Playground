//
// Created by artyomd on 1/2/20.
//

#pragma once

#include "src/api/data_type.hpp"

namespace api {
class IndexBuffer {
  unsigned int count_;
  DataType data_type_;
 public:
  IndexBuffer(unsigned int item_count, enum DataType type) : count_(item_count), data_type_(type) {}

  [[nodiscard]] inline unsigned int GetCount() const {
    return count_;
  }

  [[nodiscard]] inline enum DataType GetType() const {
    return data_type_;
  }

  virtual void Bind() const = 0;

  virtual void Unbind() const = 0;

  virtual ~IndexBuffer() = default;
};
}
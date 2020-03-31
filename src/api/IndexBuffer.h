//
// Created by artyomd on 1/2/20.
//

#pragma once

#include "DataType.h"

namespace api {
class IndexBuffer {
 private:
  unsigned int count_;
  DataType data_type_;
 public:
  IndexBuffer(unsigned int item_count, enum DataType type) : count_(item_count), data_type_(type) {

  }

  inline unsigned int GetCount() const {
    return count_;
  }

  inline enum DataType GetType() const {
    return data_type_;
  }

  virtual void Bind() const = 0;

  virtual void Unbind() const = 0;

  virtual ~IndexBuffer() = default;
};
}
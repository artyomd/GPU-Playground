//
// Created by artyomd on 1/2/20.
//

#pragma once

#include <stdexcept>
#include <vector>

#include "src/api/data_type.hpp"

namespace api {
class IndexBuffer {
 protected:
  unsigned int count_;
  DataType data_type_;
  size_t size_in_bytes_;

  IndexBuffer(unsigned int item_count, enum DataType type) :
      count_(item_count),
      data_type_(type),
      size_in_bytes_(item_count * GetDataTypeSizeInBytes(type)) {
    if (type != DataType::DATA_TYPE_UINT_16 && type != DataType::DATA_TYPE_UINT_32) {
      throw std::runtime_error("unsupported type for index buffer");
    }
  }
 public:
  virtual void Update(void *data) = 0;

  [[nodiscard]] inline unsigned int GetCount() const {
    return count_;
  }

  [[nodiscard]] inline enum DataType GetType() const {
    return data_type_;
  }

  virtual ~IndexBuffer() = default;
};
}

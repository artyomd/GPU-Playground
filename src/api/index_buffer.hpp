//
// Created by artyomd on 1/2/20.
//

#pragma once

#include <stdexcept>
#include <vector>

#include "src/api/buffer.hpp"
#include "src/api/data_type.hpp"

namespace api {
class IndexBuffer : virtual public Buffer {
 public:
  IndexBuffer() = delete;
 protected:
  IndexBuffer(unsigned int item_count, enum DataType type) :
      count_(item_count),
      data_type_(type),
      Buffer(item_count * GetDataTypeSizeInBytes(type)) {
    if (type != DataType::DATA_TYPE_UINT_16 && type != DataType::DATA_TYPE_UINT_32) {
      throw std::runtime_error("unsupported type for index buffer");
    }
  }

 public:
  [[nodiscard]] inline unsigned int GetCount() const {
    return count_;
  }

  [[nodiscard]] inline enum DataType GetType() const {
    return data_type_;
  }

  ~IndexBuffer() override = default;
 protected:
  unsigned int count_;
  DataType data_type_;
};
}

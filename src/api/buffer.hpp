//
// Created by Artyom Dangizyan on 1/1/21.
//

#pragma once

#include <cstdlib>
#include <iostream>

namespace api {
class Buffer {
 public:
  Buffer() = delete;
 protected:
  explicit Buffer(const size_t &size_in_bytes)
      : size_in_bytes_(size_in_bytes) {}
 public:
  virtual void Update(const void *data) = 0;
  virtual void CopyFrom(std::shared_ptr<api::Buffer> src_buffer, size_t size, size_t src_offset, size_t dst_offset) = 0;
  [[nodiscard]] size_t GetSizeInBytes() const {
    return size_in_bytes_;
  }
  virtual ~Buffer() = default;
 protected:
  size_t size_in_bytes_;
};
}

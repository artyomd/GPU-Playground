//
// Created by Artyom Dangizyan on 10/21/18.
//
#pragma once

#include <vector>
#include <stdexcept>
#include "DataType.h"

namespace api {
struct VertexAttribute {
  DataType type_;
  unsigned int count_;
};

class VertexBufferLayout {

 private:
  std::vector<VertexAttribute> elements_;
  unsigned int stride_;
 public:
  VertexBufferLayout() : stride_(0) {}

  template<typename T>
  inline void Push(unsigned int count) {
    throw std::runtime_error("not implemented");
  }

  [[nodiscard]] inline std::vector<VertexAttribute> GetElements() const {
    return elements_;
  }

  [[nodiscard]] inline unsigned int GetStride() const {
    return stride_;
  }
};
}
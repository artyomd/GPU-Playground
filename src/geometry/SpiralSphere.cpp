//
// Created by artyomd on 3/22/20.
//

#include <cmath>
#include <GL/glew.h>
#include "SpiralSphere.h"
#include "Point.hpp"

geometry::SpiralSphere::SpiralSphere(api::RenderingContext *context,
                                     float radius,
                                     unsigned int loops,
                                     unsigned int segmentsPerLoop)
    : GeometryItem(context) {

  auto *geometry_data = new std::vector<Point>();
  auto *index_data = new std::vector<unsigned int>();

  for (unsigned int loopSegmentNumber = 0; loopSegmentNumber < segmentsPerLoop; ++loopSegmentNumber) {
    float theta = 0;
    auto phi = static_cast<float>(loopSegmentNumber*2*M_PI/segmentsPerLoop);
    float sinTheta = std::sin(theta);
    float sinPhi = std::sin(phi);
    float cosTheta = std::cos(theta);
    float cosPhi = std::cos(phi);
    Point point = {radius*cosPhi*sinTheta, radius*sinPhi*sinTheta, radius*cosTheta,
                   1, 1, 1,1};
    geometry_data->push_back(point);

  }
  for (unsigned int loopNumber = 0; loopNumber <= loops; ++loopNumber) {
    for (unsigned int loopSegmentNumber = 0; loopSegmentNumber < segmentsPerLoop; ++loopSegmentNumber) {
      auto theta = static_cast<float>((loopNumber*M_PI/loops) +
          ((M_PI*loopSegmentNumber)/(segmentsPerLoop*loops)));
      if (loopNumber==loops) {
        theta = static_cast<float>(M_PI);
      }
      auto phi = static_cast<float>(loopSegmentNumber*2*M_PI/segmentsPerLoop);
      float sinTheta = std::sin(theta);
      float sinPhi = std::sin(phi);
      float cosTheta = std::cos(theta);
      float cosPhi = std::cos(phi);
      Point point = {radius*cosPhi*sinTheta, radius*sinPhi*sinTheta, radius*cosTheta,
                     1, 1, 1,1};
      geometry_data->push_back(point);

    }
  }
  for (unsigned int loopSegmentNumber = 0; loopSegmentNumber < segmentsPerLoop; ++loopSegmentNumber) {
    index_data->push_back(static_cast<unsigned short &&>(loopSegmentNumber));
    index_data->push_back(static_cast<unsigned short &&>(segmentsPerLoop + loopSegmentNumber));
  }
  for (unsigned int loopNumber = 0; loopNumber < loops; ++loopNumber) {
    for (unsigned int loopSegmentNumber = 0; loopSegmentNumber < segmentsPerLoop; ++loopSegmentNumber) {
      index_data->push_back(
          static_cast<unsigned short &&>(((loopNumber + 1)*segmentsPerLoop) + loopSegmentNumber));
      index_data->push_back(
          static_cast<unsigned short &&>(((loopNumber + 2)*segmentsPerLoop) + loopSegmentNumber));
    }
  }

  vertex_buffer_ =
      context->CreateVertexBuffer(geometry_data->data(),
                                  static_cast<unsigned int>(7*geometry_data->size()*sizeof(float)));
  layout_ = new api::VertexBufferLayout();
  layout_->Push<float>(3);
  layout_->Push<float>(4);

  vertex_binding_ = context->CreateVertexBinding(vertex_buffer_, layout_);
  index_buffer_ = context->CreateIndexBuffer(index_data->data(),
                                             static_cast<unsigned int>(index_data->size()),
                                             api::DATA_TYPE_UINT_32);
  delete index_data;
  delete geometry_data;

}
geometry::SpiralSphere::~SpiralSphere() {
  context_->FreeIndexBuffer(index_buffer_);
  context_->FreeVertexBiding(vertex_binding_);
  delete (layout_);
  context_->FreeVertexBuffer(vertex_buffer_);
}

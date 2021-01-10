//
// Created by Artyom Dangizyan on 12/30/20.
//

#include "src/geometry/gltf_model.hpp"

#include <tinygltf/tiny_gltf.h>
#include <utility>
#include <glm/ext.hpp>

static api::DataType GetType(int type) {
  switch (type) {
    case TINYGLTF_COMPONENT_TYPE_BYTE:return api::DataType::DATA_TYPE_BYTE;
    case TINYGLTF_COMPONENT_TYPE_FLOAT:return api::DataType::DATA_TYPE_FLOAT;
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:return api::DataType::DATA_TYPE_UINT_16;
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:return api::DataType::DATA_TYPE_UINT_32;
    default: throw std::runtime_error("unknown type");
  }
}

static api::DrawMode GetMode(int mode) {
  switch (mode) {
    case TINYGLTF_MODE_POINTS:return api::DrawMode::POINT_LIST;
    case TINYGLTF_MODE_LINE:return api::DrawMode::LINE_LIST;
    case TINYGLTF_MODE_LINE_STRIP:return api::DrawMode::LINE_STRIP;
    case TINYGLTF_MODE_TRIANGLES:return api::DrawMode::TRIANGLE_LIST;
    case TINYGLTF_MODE_TRIANGLE_FAN:return api::DrawMode::TRIANGLE_FAN;
    case TINYGLTF_MODE_TRIANGLE_STRIP:return api::DrawMode::TRIANGLE_STRIP;
    default: throw std::runtime_error("unknown mode");
  }
}

static api::Filter GetFilter(int filter) {
  switch (filter) {
    case TINYGLTF_TEXTURE_FILTER_LINEAR:
    case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR:
    case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_NEAREST:return api::Filter::LINEAR;
    case TINYGLTF_TEXTURE_FILTER_NEAREST:
    case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_NEAREST:
    case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_LINEAR:return api::Filter::NEAREST;
    default: throw std::runtime_error("unknown filter");
  }
}

static api::AddressMode GetAddressMode(int address_mode) {
  switch (address_mode) {
    case TINYGLTF_TEXTURE_WRAP_CLAMP_TO_EDGE:return api::AddressMode::CLAMP_TO_EDGE;
    case TINYGLTF_TEXTURE_WRAP_MIRRORED_REPEAT:return api::AddressMode::MIRRORED_REPEAT;
    case TINYGLTF_TEXTURE_WRAP_REPEAT:return api::AddressMode::REPEAT;
    default: throw std::runtime_error("unknown address mode");
  }
}

static api::Sampler GetSampler(const tinygltf::Sampler &sampler) {
  return {
      GetFilter(sampler.magFilter),
      GetFilter(sampler.minFilter),
      GetAddressMode(sampler.wrapS),
      GetAddressMode(sampler.wrapT),
      api::AddressMode::REPEAT
  };
}

namespace geometry {
struct PrimitiveUbo {
  alignas(16) glm::vec3 max;
  alignas(16) glm::vec3 min;
  alignas(16) glm::mat4 model;
  alignas(16) glm::mat4 view;
  alignas(16) glm::mat4 projection;
};
}

geometry::GltfModel::GltfModel(std::shared_ptr<api::RenderingContext> context, const std::string &path)
    : context_(std::move(context)) {
  tinygltf::TinyGLTF loader;
  std::string err;
  std::string warn;

  bool ret = loader.LoadASCIIFromFile(&model_, &err, &warn, path);

  if (!warn.empty()) {
    (void) printf("Warn: %s\n", warn.c_str());
  }

  if (!err.empty()) {
    (void) printf("Err: %s\n", err.c_str());
  }

  if (!ret) {
    throw std::runtime_error("Failed to parse glTF\n");
  }

  for (const auto &buffer:model_.buffers) {
    auto gpu_buffer = context_->CreateBuffer(buffer.data.size());
    gpu_buffer->Update(buffer.data.data());
    buffers_.push_back(gpu_buffer);
  }

  for (const auto &image:model_.images) {
    auto texture = context_->CreateTexture2D();
    texture->Load(image.width, image.height, image.image.data());
    texture->SetSampler(GetSampler(model_.samplers[0]));
    textures_.emplace_back(texture);
  }
}

void geometry::GltfModel::LoadScene() {
  current_pipelines_.clear();
  auto default_scene = model_.scenes[model_.defaultScene];
  for (auto node:default_scene.nodes) {
    LoadNode(model_.nodes[node]);
  }
  SetCamera(0);
}

void geometry::GltfModel::LoadNode(const tinygltf::Node &node, glm::mat4 parent_transform) {
  glm::mat4 matrix = glm::mat4(1.0F);
  if (!node.matrix.empty()) {
    matrix = glm::make_mat4(node.matrix.data());
  } else {
    auto translation = glm::vec3(1.0);
    auto rotation = glm::mat4(1.0);
    auto scale = glm::vec3(1.0);
    if (!node.translation.empty()) {
      translation = glm::make_vec3(node.translation.data());
    }
    if (!node.rotation.empty()) {
      rotation = glm::mat4(static_cast<glm::quat>(glm::make_quat(node.rotation.data())));
    }
    if (!node.scale.empty()) {
      scale = glm::make_vec3(node.scale.data());
    }
    matrix = glm::scale(matrix, scale);
    matrix = matrix * rotation;
    matrix = glm::translate(matrix, translation);
  }
  matrix = parent_transform * matrix;
  if (node.mesh != -1) {
    auto pipes = LoadMesh(model_.meshes[node.mesh], matrix);
    current_pipelines_.insert(current_pipelines_.end(), pipes.begin(), pipes.end());
  }
  if (node.camera != -1) {
    auto camera = model_.cameras[node.camera];
    glm::mat4 camera_matrix = glm::mat4(1.0);
    if (camera.type == "perspective") {
      auto data = camera.perspective;
      camera_matrix = glm::perspective(data.yfov, data.aspectRatio, data.znear, data.zfar);
    } else {
      auto data = camera.orthographic;
      camera_matrix[0][0] = 1.0F / data.xmag;
      camera_matrix[1][1] = 1.0F / data.ymag;
      camera_matrix[2][2] = 2.0F / (data.znear - data.zfar);
      camera_matrix[3][2] = (data.zfar + data.znear) / (data.znear - data.zfar);
    }
    Camera camera_obj{matrix, camera_matrix, camera.name};
    cameras_.emplace_back(camera_obj);
  }
  for (auto child:node.children) {
    LoadNode(model_.nodes[child], matrix);
  }
}

std::vector<geometry::RenderingUnit> geometry::GltfModel::LoadMesh(tinygltf::Mesh &mesh,
                                                                   glm::mat4 model_matrix) {
  std::vector<geometry::RenderingUnit> pipelines{};
  for (const auto &primitive:mesh.primitives) {
    auto ubo = std::make_shared<PrimitiveUbo>();
    std::shared_ptr<api::IndexBuffer> index_buffer;
    std::shared_ptr<api::VertexBuffer> vertex_buffer;
    unsigned int index_max_value;
    { //index buffer
      if (primitive.indices == -1) {
        throw std::runtime_error("panic");
      } else {
        auto indices_accessor = model_.accessors[primitive.indices];
        if (indices_accessor.type != TINYGLTF_TYPE_SCALAR) {
          throw std::runtime_error("wrong type for index buffer");
        }
        if (indices_accessor.byteOffset != 0) {
          throw std::runtime_error("panic");
        }
        if (indices_accessor.normalized) {
          throw std::runtime_error("panic");
        }
        auto type = GetType(indices_accessor.componentType);
        auto accessor_defined_length = indices_accessor.count * api::GetDataTypeSizeInBytes(type);
        auto buffer_view = model_.bufferViews[indices_accessor.bufferView];
        if (accessor_defined_length != buffer_view.byteLength) {
          throw std::runtime_error("panic");
        }
        if (buffer_view.byteStride != 0) {
          throw std::runtime_error("panic");
        }
        if (buffer_view.target != TINYGLTF_TARGET_ELEMENT_ARRAY_BUFFER) {
          throw std::runtime_error("panic");
        }
        index_buffer = context_->CreateIndexBuffer(indices_accessor.count, type);
        auto buffer = buffers_[buffer_view.buffer];
        index_buffer->CopyFrom(buffer, index_buffer->GetSizeInBytes(), buffer_view.byteOffset, 0);
        index_max_value = static_cast<unsigned int>(indices_accessor.maxValues[0]);
      }
    }
    {//vertex buffer
      auto vbl = api::VertexBufferLayout();
      size_t current_offset = 0;
      std::map<std::string, std::shared_ptr<api::Buffer>> attribute_buffer_mapping{};
      auto position = primitive.attributes.find("POSITION");
      if (position != primitive.attributes.end()) {
        auto position_accessor = model_.accessors[position->second];
        if (position_accessor.count != index_max_value + 1) {
          throw std::runtime_error("panic");
        }
        if (position_accessor.type != TINYGLTF_TYPE_VEC3) {
          throw std::runtime_error("panic");
        }
        auto type = GetType(position_accessor.componentType);
        if (type != api::DataType::DATA_TYPE_FLOAT) {
          throw std::runtime_error("panic");
        }
        auto position_buffer_view = model_.bufferViews[position_accessor.bufferView];
        if (position_buffer_view.target != TINYGLTF_TARGET_ARRAY_BUFFER) {
          throw std::runtime_error("panic");
        }
        auto buffer_view = model_.bufferViews[position_accessor.bufferView];
        auto position_size_in_bytes = position_accessor.count * api::GetDataTypeSizeInBytes(type) * 3;
        if (buffer_view.byteStride != api::GetDataTypeSizeInBytes(type) * 3) {
          throw std::runtime_error("panic");
        }
        auto buffer = context_->CreateBuffer(position_size_in_bytes);
        buffer->Update(
            model_.buffers[buffer_view.buffer].data.data() + buffer_view.byteOffset + position_accessor.byteOffset);
        for (int i = 0; i < position_accessor.maxValues.size(); i++) {
          ubo->max[i] = position_accessor.maxValues[i];
        }
        for (int i = 0; i < position_accessor.minValues.size(); i++) {
          ubo->min[i] = position_accessor.minValues[i];
        }
        ubo->model = model_matrix;
        vbl.Push(api::VertexAttribute{type, 3, current_offset, buffer_view.byteStride});
        attribute_buffer_mapping.insert(std::make_pair("POSITION", buffer));
        current_offset += buffer->GetSizeInBytes();
      }
      auto coords = primitive.attributes.find("TEXCOORD_0");
      if (coords != primitive.attributes.end()) {
        auto coords_accessor = model_.accessors[coords->second];
        if (coords_accessor.count != index_max_value + 1) {
          throw std::runtime_error("panic");
        }
        if (coords_accessor.type != TINYGLTF_TYPE_VEC2) {
          throw std::runtime_error("panic");
        }
        auto type = GetType(coords_accessor.componentType);
        if (type != api::DataType::DATA_TYPE_FLOAT) {
          throw std::runtime_error("panic");
        }
        auto position_buffer_view = model_.bufferViews[coords_accessor.bufferView];
        if (position_buffer_view.target != TINYGLTF_TARGET_ARRAY_BUFFER) {
          throw std::runtime_error("panic");
        }
        auto buffer_view = model_.bufferViews[coords_accessor.bufferView];
        auto position_size_in_bytes = coords_accessor.count * api::GetDataTypeSizeInBytes(type) * 2;
        if (buffer_view.byteStride != api::GetDataTypeSizeInBytes(type) * 2) {
          throw std::runtime_error("panic");
        }
        auto buffer = context_->CreateBuffer(position_size_in_bytes);
        buffer->Update(
            model_.buffers[buffer_view.buffer].data.data() + buffer_view.byteOffset + coords_accessor.byteOffset);
        vbl.Push(api::VertexAttribute{type, 2, current_offset, buffer_view.byteStride});
        attribute_buffer_mapping.insert(std::make_pair("TEXCOORD_0", buffer));
        current_offset += buffer->GetSizeInBytes();
      }
      vertex_buffer = context_->CreateVertexBuffer(current_offset, vbl);
      auto pos_buffer = attribute_buffer_mapping["POSITION"];
      auto coords_buffer = attribute_buffer_mapping["TEXCOORD_0"];
      vertex_buffer->CopyFrom(pos_buffer, pos_buffer->GetSizeInBytes(), 0, 0);
      vertex_buffer->CopyFrom(coords_buffer, coords_buffer->GetSizeInBytes(), 0, pos_buffer->GetSizeInBytes());
    }
    auto vertex_shader = context_->CreateShader("../res/shader/compiled/gltf_vertex.spv",
                                                "main",
                                                api::ShaderType::SHADER_TYPE_VERTEX);
    auto fragment_shader = context_->CreateShader("../res/shader/compiled/gltf_fragment.spv",
                                                  "main",
                                                  api::ShaderType::SHADER_TYPE_FRAGMENT);
    auto pipe = context_->CreateGraphicsPipeline(vertex_buffer, index_buffer, vertex_shader, fragment_shader, {
        GetMode(primitive.mode),
        api::CullMode::BACK,
        api::FrontFace::CCW,
        true,
        api::CompareOp::LESS,
    });
    pipe->SetTexture(1, this->textures_[0]);
    RenderingUnit unit{pipe, ubo};
    pipelines.emplace_back(unit);
  }
  return pipelines;
}

void geometry::GltfModel::Render() {
  for (const auto &unit:current_pipelines_) {
    unit.pipeline->Render();
  }
}
void geometry::GltfModel::SetViewport(size_t width, size_t height) {
  for (const auto &unit:current_pipelines_) {
    unit.pipeline->SetViewPort(width, height);
  }
}

void geometry::GltfModel::SetCamera(int camera_index) {
  auto camera = cameras_[camera_index];
  for (const auto &unit:current_pipelines_) {
    unit.ubo->projection = camera.proj;
    unit.ubo->view = glm::inverse(camera.view);
    unit.pipeline->UpdateUniformBuffer(0, unit.ubo.get());
  }
}

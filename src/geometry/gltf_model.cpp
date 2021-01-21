//
// Created by Artyom Dangizyan on 12/30/20.
//

#include "src/geometry/gltf_model.hpp"

#include <utility>
#include <glm/ext.hpp>
#include <snowhouse/snowhouse.h>
#include <tinygltf/tiny_gltf.h>

#include "src/geometry/gltf_model_defaults.hpp"

namespace geometry {
struct ParsedAttribute {
  unsigned char *data = nullptr;
  size_t stride = 0;
  size_t instance_count = 0;
  unsigned int element_count = 0;
  api::DataType element_type = api::DataType::DATA_TYPE_FLOAT;
  size_t element_size = 0;
};
}

static api::DataType GetType(int type) {
  switch (type) {
    case TINYGLTF_COMPONENT_TYPE_BYTE:return api::DataType::DATA_TYPE_BYTE;
    case TINYGLTF_COMPONENT_TYPE_FLOAT:return api::DataType::DATA_TYPE_FLOAT;
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:return api::DataType::DATA_TYPE_UINT_16;
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:return api::DataType::DATA_TYPE_UINT_32;
    default: throw std::runtime_error("unknown type");
  }
}

static unsigned int GetElementCount(int type) {
  switch (type) {
    case TINYGLTF_TYPE_SCALAR:return 1;
    case TINYGLTF_TYPE_VEC2:return 2;
    case TINYGLTF_TYPE_VEC3:return 3;
    case TINYGLTF_TYPE_VEC4:return 4;
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
    case -1: return api::Filter::LINEAR;
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

  for (const auto &tex:model_.textures) {
    auto texture = context_->CreateTexture2D();
    auto image = model_.images[static_cast<unsigned long>(tex.source)];
    texture->Load(static_cast<size_t>(image.width), static_cast<size_t>(image.height), image.image.data());
    texture->SetSampler(GetSampler(model_.samplers[static_cast<unsigned long>(tex.sampler)]));
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
      camera_matrix[0][0] = static_cast<float>(1.0F / data.xmag);
      camera_matrix[1][1] = static_cast<float>(1.0F / data.ymag);
      camera_matrix[2][2] = static_cast<float>(2.0F / (data.znear - data.zfar));
      camera_matrix[3][2] = static_cast<float>((data.zfar + data.znear) / (data.znear - data.zfar));
    }
    Camera camera_obj{matrix, camera_matrix, camera.name};
    cameras_.emplace_back(camera_obj);
  }
  for (const auto &child:node.children) {
    LoadNode(model_.nodes[child], matrix);
  }
}

std::vector<geometry::RenderingUnit> geometry::GltfModel::LoadMesh(tinygltf::Mesh &mesh,
                                                                   glm::mat4 model_matrix) {
  std::vector<geometry::RenderingUnit> pipelines{};
  for (const auto &primitive:mesh.primitives) {
    auto ubo = std::make_shared<PrimitiveUbo>();
    ubo->model = model_matrix;
    std::shared_ptr<api::IndexBuffer> index_buffer;
    std::shared_ptr<api::VertexBuffer> vertex_buffer;
    bool has_normals = false;
    { //index buffer
      AssertThat(primitive.indices, snowhouse::Is().Not().EqualTo(-1)); //unhandled
      auto index = ParseAttribute("INDICES", primitive.indices);
      char *data = new char[index.instance_count * index.element_size];
      size_t offset = 0;
      for (unsigned long i = 0; i < index.instance_count; i++) {
        memcpy(data + offset,
               index.data + (i * index.stride),
               index.element_size);
        offset += index.element_size;
      }
      index_buffer = context_->CreateIndexBuffer(index.instance_count, index.element_type);
      index_buffer->Update(data);
      delete[] data;
    }
    {
      auto attributes = primitive.attributes;
      auto vbl = api::VertexBufferLayout();
      ParsedAttribute position{};
      ParsedAttribute normal{};
      ParsedAttribute tangent{};
      ParsedAttribute text_coord_0{};
      ParsedAttribute text_coord_1{};
      ParsedAttribute color_0{};
      ParsedAttribute joints_0{};
      ParsedAttribute weights_0{};

      auto position_attr = attributes.find("POSITION");
      AssertThat(position_attr, snowhouse::Is().Not().EqualTo(attributes.end()));
      position = ParseAttribute("POSITION", position_attr->second);
      auto vertex_count = position.instance_count;
      vbl.Push({0, position.element_type, position.element_count});

      if (attributes.find("NORMAL") != attributes.end()) {
        normal = ParseAttribute("NORMAL", attributes.find("NORMAL")->second);
        AssertThat(normal.instance_count, snowhouse::Is().EqualTo(vertex_count));
        vbl.Push({1, normal.element_type, normal.element_count});
        has_normals = true;
      }
      if (attributes.find("TANGENT") != attributes.end()) {
        tangent = ParseAttribute("TANGENT", attributes.find("TANGENT")->second);
        AssertThat(normal.instance_count, snowhouse::Is().EqualTo(vertex_count));
        vbl.Push({2, tangent.element_type, tangent.element_count});
      }
      if (attributes.find("TEXCOORD_0") != attributes.end()) {
        text_coord_0 = ParseAttribute("TEXCOORD_0", attributes.find("TEXCOORD_0")->second);
        AssertThat(text_coord_0.instance_count, snowhouse::Is().EqualTo(vertex_count));
        vbl.Push({3, text_coord_0.element_type, text_coord_0.element_count});
      }
      if (attributes.find("TEXCOORD_1") != attributes.end()) {
        text_coord_1 = ParseAttribute("TEXCOORD_1", attributes.find("TEXCOORD_1")->second);
        AssertThat(text_coord_1.instance_count, snowhouse::Is().EqualTo(vertex_count));
        vbl.Push({4, text_coord_1.element_type, text_coord_1.element_count});
      }
      if (attributes.find("COLOR_0") != attributes.end()) {
        color_0 = ParseAttribute("TEXCOORD_1", attributes.find("TEXCOORD_1")->second);
        AssertThat(color_0.instance_count, snowhouse::Is().EqualTo(vertex_count));
        vbl.Push({5, color_0.element_type, color_0.element_count});
      }
      if (attributes.find("JOINTS_0") != attributes.end()) {
        joints_0 = ParseAttribute("JOINTS_0", attributes.find("JOINTS_0")->second);
        AssertThat(joints_0.instance_count, snowhouse::Is().EqualTo(vertex_count));
        vbl.Push({6, joints_0.element_type, joints_0.element_count});
      }
      if (attributes.find("WEIGHTS_0") != attributes.end()) {
        weights_0 = ParseAttribute("WEIGHTS_0", attributes.find("WEIGHTS_0")->second);
        AssertThat(weights_0.instance_count, snowhouse::Is().EqualTo(vertex_count));
        vbl.Push({7, weights_0.element_type, weights_0.element_count});
      }
      auto vertex_stride = vbl.GetElementSize();
      char *vertex_data = new char[vertex_count * vertex_stride];
      size_t vertex_data_offset = 0;
      for (unsigned long i = 0; i < vertex_count; i++) {
        memcpy(vertex_data + vertex_data_offset,
               position.data + (i * position.stride),
               position.element_size);
        vertex_data_offset += position.element_size;
        if (has_normals) {
          memcpy(vertex_data + vertex_data_offset,
                 normal.data + (i * normal.stride),
                 normal.element_size);
          vertex_data_offset += normal.element_size;
        }
        if (tangent.data != nullptr) {
          memcpy(vertex_data + vertex_data_offset,
                 tangent.data + (i * tangent.stride),
                 tangent.element_size);
          vertex_data_offset += tangent.element_size;
        }
        if (text_coord_0.data != nullptr) {
          memcpy(vertex_data + vertex_data_offset,
                 text_coord_0.data + (i * text_coord_0.stride),
                 text_coord_0.element_size);
          vertex_data_offset += text_coord_0.element_size;
        }
        if (text_coord_1.data != nullptr) {
          memcpy(vertex_data + vertex_data_offset,
                 text_coord_1.data + (i * text_coord_1.stride),
                 text_coord_1.element_size);
          vertex_data_offset += text_coord_1.element_size;
        }
        if (color_0.data != nullptr) {
          memcpy(vertex_data + vertex_data_offset,
                 color_0.data + (i * color_0.stride),
                 color_0.element_size);
          vertex_data_offset += color_0.element_size;
        }
        if (joints_0.data != nullptr) {
          memcpy(vertex_data + vertex_data_offset,
                 joints_0.data + (i * joints_0.stride),
                 joints_0.element_size);
          vertex_data_offset += joints_0.element_size;
        }
        if (weights_0.data != nullptr) {
          memcpy(vertex_data + vertex_data_offset,
                 weights_0.data + (i * weights_0.stride),
                 weights_0.element_size);
          vertex_data_offset += weights_0.element_size;
        }
      }
      vertex_buffer = context_->CreateVertexBuffer(vertex_count * vertex_stride, vbl);
      vertex_buffer->Update(vertex_data);
      delete[] vertex_data;
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
geometry::ParsedAttribute geometry::GltfModel::ParseAttribute(const std::string &attribute_name,
                                                              int accessor_id) {
  auto accessor = model_.accessors[static_cast<unsigned long>(accessor_id)];
  auto attr_defaults = geometry::kPrimitiveTypes.find(attribute_name);
  auto expected_count = attr_defaults->second.element_count;
  bool matched = false;
  for (auto type:expected_count) {
    if (accessor.type == type) {
      matched = true;
      break;
    }
  }
  AssertThat(matched, snowhouse::IsTrue());
  auto expected_data_types = attr_defaults->second.element_type;
  matched = false;
  for (auto type:expected_data_types) {
    if (accessor.componentType == type) {
      matched = true;
      break;
    }
  }
  AssertThat(matched, snowhouse::IsTrue());
  auto data_type = GetType(accessor.componentType);
  auto buffer_view = model_.bufferViews[static_cast<unsigned long>(accessor.bufferView)];
  AssertThat(buffer_view.target, snowhouse::Is().EqualTo(attr_defaults->second.target));
  auto stride_in_current_buffer = buffer_view.byteStride;
  unsigned char *data = model_.buffers[static_cast<unsigned long>(buffer_view.buffer)].data.data()
      + buffer_view.byteOffset
      + accessor.byteOffset;
  auto element_count = GetElementCount(accessor.type);
  auto element_size = element_count * api::GetDataTypeSizeInBytes(data_type);
  if (stride_in_current_buffer == 0) {
    stride_in_current_buffer = element_size;
  }
  return {data,
          stride_in_current_buffer,
          accessor.count,
          element_count,
          data_type,
          element_size
  };
}

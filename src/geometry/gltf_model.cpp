//
// Created by Artyom Dangizyan on 12/30/20.
//

#include "src/geometry/gltf_model.hpp"

#include <spdlog/spdlog.h>
#include <tinygltf/tiny_gltf.h>

#include <glm/ext.hpp>
#include <utility>

#include "src/geometry/gltf_model_defaults.hpp"
#include "src/shaders/shaders.hpp"
#include "src/utils/check.hpp"
namespace {
api::DataType GetType(int type) {
  switch (type) {
    case TINYGLTF_COMPONENT_TYPE_BYTE:
      return api::DataType::BYTE;
    case TINYGLTF_COMPONENT_TYPE_FLOAT:
      return api::DataType::FLOAT;
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
      return api::DataType::UINT_16;
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
      return api::DataType::UINT_32;
    default:
      throw std::runtime_error("unknown type");
  }
}

unsigned int GetElementCount(int type) {
  switch (type) {
    case TINYGLTF_TYPE_SCALAR:
      return 1;
    case TINYGLTF_TYPE_VEC2:
      return 2;
    case TINYGLTF_TYPE_VEC3:
      return 3;
    case TINYGLTF_TYPE_VEC4:
      return 4;
    default:
      throw std::runtime_error("unknown type");
  }
}

api::DrawMode GetMode(int mode) {
  switch (mode) {
    case TINYGLTF_MODE_POINTS:
      return api::DrawMode::POINT_LIST;
    case TINYGLTF_MODE_LINE:
      return api::DrawMode::LINE_LIST;
    case TINYGLTF_MODE_LINE_STRIP:
      return api::DrawMode::LINE_STRIP;
    case TINYGLTF_MODE_TRIANGLES:
      return api::DrawMode::TRIANGLE_LIST;
    case TINYGLTF_MODE_TRIANGLE_FAN:
      return api::DrawMode::TRIANGLE_FAN;
    case TINYGLTF_MODE_TRIANGLE_STRIP:
      return api::DrawMode::TRIANGLE_STRIP;
    default:
      throw std::runtime_error("unknown mode");
  }
}

api::Filter GetFilter(int filter) {
  switch (filter) {
    case TINYGLTF_TEXTURE_FILTER_LINEAR:
    case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR:
    case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_NEAREST:
      return api::Filter::LINEAR;
    case TINYGLTF_TEXTURE_FILTER_NEAREST:
    case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_NEAREST:
    case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_LINEAR:
      return api::Filter::NEAREST;
    case -1:
      return api::Filter::LINEAR;
    default:
      throw std::runtime_error("unknown filter");
  }
}

api::AddressMode GetAddressMode(int address_mode) {
  switch (address_mode) {
    case TINYGLTF_TEXTURE_WRAP_CLAMP_TO_EDGE:
      return api::AddressMode::CLAMP_TO_EDGE;
    case TINYGLTF_TEXTURE_WRAP_MIRRORED_REPEAT:
      return api::AddressMode::MIRRORED_REPEAT;
    case TINYGLTF_TEXTURE_WRAP_REPEAT:
      return api::AddressMode::REPEAT;
    default:
      throw std::runtime_error("unknown address mode");
  }
}

api::PixelFormat GetPixelFormat(bool srgb, const tinygltf::Image &image) {
  CHECK(image.pixel_type == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE,
        "pixel type bust be ubyte")
  CHECK(image.component == 4, "only 4 channel images are supported")
  if (srgb) {
    return api::PixelFormat::RGBA_8_SRGB;
  } else {
    return api::PixelFormat::RGBA_8_UNORM;
  }
}

api::Sampler GetSampler(const tinygltf::Sampler &sampler) {
  return {GetFilter(sampler.magFilter), GetFilter(sampler.minFilter),
          GetAddressMode(sampler.wrapS), GetAddressMode(sampler.wrapT),
          api::AddressMode::REPEAT};
}
}  // namespace
namespace geometry {
struct ParsedAttribute {
  unsigned char *data = nullptr;
  size_t stride = 0;
  size_t instance_count = 0;
  unsigned int element_count = 0;
  api::DataType element_type = api::DataType::FLOAT;
  size_t element_size = 0;
};
struct MVP {
  alignas(16) glm::mat4 model;
  alignas(16) glm::mat4 view;
  alignas(16) glm::mat4 projection;
  alignas(16) glm::mat4 normal;
};

struct Material {
  alignas(16) glm::vec4 base_color;
  alignas(16) glm::vec3 emissive_factor;
  alignas(4) float metallic_factor;
  alignas(4) float roughness_factor;
  alignas(4) float alpha_cutoff;
  alignas(4) bool double_sided;
};
struct PrimitiveUbo {
  MVP mvp{};
  Material material{};
};
}  // namespace geometry

geometry::GltfModel::GltfModel(std::shared_ptr<api::RenderingContext> context,
                               const std::string &path)
    : context_(std::move(context)) {
  tinygltf::TinyGLTF loader;
  std::string err;
  std::string warn;

  bool ret = loader.LoadASCIIFromFile(&model_, &err, &warn, path);

  if (!warn.empty()) {
    spdlog::warn(warn);
  }

  if (!err.empty()) {
    spdlog::error(warn);
  }

  if (!ret) {
    throw std::runtime_error("Failed to parse glTF\n");
  }

  for (const auto &camera : model_.cameras) {
    glm::mat4 camera_matrix = glm::mat4(1.0);
    if (camera.type == "perspective") {
      auto data = camera.perspective;
      camera_matrix =
          glm::perspective(data.yfov, data.aspectRatio, data.znear, data.zfar);
    } else {
      auto data = camera.orthographic;
      camera_matrix[0][0] = static_cast<float>(1.0F / data.xmag);
      camera_matrix[1][1] = static_cast<float>(1.0F / data.ymag);
      camera_matrix[2][2] = static_cast<float>(2.0F / (data.znear - data.zfar));
      camera_matrix[3][2] = static_cast<float>((data.zfar + data.znear) /
                                               (data.znear - data.zfar));
    }
    cameras_.emplace_back(camera_matrix);
  }
  for (const auto &tex : model_.textures) {
    auto image = this->model_.images[static_cast<size_t>(tex.source)];
    auto texture = context_->CreateTexture2D(
        static_cast<uint32_t>(image.width), static_cast<uint32_t>(image.height),
        GetPixelFormat(true, image));
    texture->Load(image.image.data());
    texture->SetSampler(
        GetSampler(this->model_.samplers[static_cast<size_t>(tex.sampler)]));
    textures_.push_back(texture);
  }
}

void geometry::GltfModel::LoadScene(uint scene_index) {
  context_->WaitForGpuIdle();
  current_pipelines_.clear();
  CHECK(model_.scenes.size() != 0u, "model must have at least 1 scene")
  CHECK(scene_index < model_.scenes.size(),
        "scene with given index does not exists")
  auto default_scene = model_.scenes[static_cast<size_t>(scene_index)];
  for (const auto &node : default_scene.nodes) {
    LoadNode(model_.nodes[static_cast<size_t>(node)]);
  }
}

void geometry::GltfModel::LoadNode(const tinygltf::Node &node,
                                   glm::mat4 parent_transform) {
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
      rotation = glm::mat4(
          static_cast<glm::quat>(glm::make_quat(node.rotation.data())));
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
    auto pipes =
        LoadMesh(model_.meshes[static_cast<size_t>(node.mesh)], matrix);
    current_pipelines_.insert(current_pipelines_.end(), pipes.begin(),
                              pipes.end());
  }
  if (node.camera != -1) {
    SetCamera(static_cast<uint>(node.camera), glm::inverse(matrix));
  }
  for (const auto &child : node.children) {
    LoadNode(model_.nodes[static_cast<size_t>(child)], matrix);
  }
}

std::vector<geometry::RenderingUnit> geometry::GltfModel::LoadMesh(
    tinygltf::Mesh &mesh, glm::mat4 model_matrix) {
  std::vector<geometry::RenderingUnit> pipelines{};
  for (const auto &primitive : mesh.primitives) {
    auto ubo = std::make_shared<PrimitiveUbo>();
    ubo->mvp.model = model_matrix;
    ubo->mvp.normal = glm::inverseTranspose(model_matrix);
    std::shared_ptr<api::Buffer> index_buffer;
    api::DataType index_buffer_type;
    size_t index_count;
    std::shared_ptr<api::Buffer> vertex_buffer;
    auto vbl = api::VertexBufferLayout();
    bool has_normals = false;
    bool has_tangents = false;
    bool has_text_coord_0 = false;
    bool has_text_coord_1 = false;
    bool has_color_0 = false;
    bool has_joints_0 = false;
    bool has_weights_0 = false;

    {                                              // index buffer
      CHECK(primitive.indices == -1, "unhnadled")  // unhandled
      auto index = ParseAttribute("INDICES", primitive.indices);
      char *data = new char[index.instance_count * index.element_size];
      size_t offset = 0;
      for (unsigned long i = 0; i < index.instance_count; i++) {
        memcpy(data + offset, index.data + (i * index.stride),
               index.element_size);
        offset += index.element_size;
      }
      index_buffer = context_->CreateBuffer(
          static_cast<uint32_t>(index.instance_count) *
              api::GetDataTypeSizeInBytes(index.element_type),
          api::BufferUsage::INDEX_BUFFER, api::MemoryType::DEVICE_LOCAL);
      index_buffer_type = index.element_type;
      index_count = index.instance_count;
      index_buffer->Update(data);
      delete[] data;
    }
    {
      auto attributes = primitive.attributes;
      ParsedAttribute position{};
      ParsedAttribute normal{};
      ParsedAttribute tangent{};
      ParsedAttribute text_coord_0{};
      ParsedAttribute text_coord_1{};
      ParsedAttribute color_0{};
      ParsedAttribute joints_0{};
      ParsedAttribute weights_0{};

      auto position_attr = attributes.find("POSITION");
      CHECK(position_attr != attributes.end(),
            "must contain position attribute")
      position = ParseAttribute("POSITION", position_attr->second);
      auto vertex_count = position.instance_count;
      vbl.Push({0, position.element_type, position.element_count});

      if (attributes.find("NORMAL") != attributes.end()) {
        normal = ParseAttribute("NORMAL", attributes.find("NORMAL")->second);

        vbl.Push({1, normal.element_type, normal.element_count});
        has_normals = true;
      }
      if (attributes.find("TANGENT") != attributes.end()) {
        tangent = ParseAttribute("TANGENT", attributes.find("TANGENT")->second);
        CHECK(normal.instance_count == vertex_count, "unhandled")
        vbl.Push({2, tangent.element_type, tangent.element_count});
        has_tangents = true;
      }
      if (attributes.find("TEXCOORD_0") != attributes.end()) {
        text_coord_0 =
            ParseAttribute("TEXCOORD_0", attributes.find("TEXCOORD_0")->second);
        CHECK(text_coord_0.instance_count = vertex_count, "unhandled");
        vbl.Push({3, text_coord_0.element_type, text_coord_0.element_count});
        has_text_coord_0 = true;
      }
      if (attributes.find("TEXCOORD_1") != attributes.end()) {
        text_coord_1 =
            ParseAttribute("TEXCOORD_1", attributes.find("TEXCOORD_1")->second);

        CHECK(text_coord_1.instance_count = vertex_count, "unhandled");
        vbl.Push({4, text_coord_1.element_type, text_coord_1.element_count});
        has_text_coord_1 = true;
      }
      if (attributes.find("COLOR_0") != attributes.end()) {
        color_0 =
            ParseAttribute("TEXCOORD_1", attributes.find("TEXCOORD_1")->second);
        CHECK(color_0.instance_count == vertex_count, "unhandled");
        vbl.Push({5, color_0.element_type, color_0.element_count});
        has_color_0 = true;
      }
      if (attributes.find("JOINTS_0") != attributes.end()) {
        joints_0 =
            ParseAttribute("JOINTS_0", attributes.find("JOINTS_0")->second);
        CHECK(joints_0.instance_count == vertex_count, "unhandled");
        vbl.Push({6, joints_0.element_type, joints_0.element_count});
        has_joints_0 = true;
      }
      if (attributes.find("WEIGHTS_0") != attributes.end()) {
        weights_0 =
            ParseAttribute("WEIGHTS_0", attributes.find("WEIGHTS_0")->second);
        CHECK(weights_0.instance_count == vertex_count, "unhandled");
        vbl.Push({7, weights_0.element_type, weights_0.element_count});
        has_weights_0 = true;
      }
      auto vertex_stride = vbl.GetElementSize();
      char *vertex_data =
          static_cast<char *>(malloc(vertex_count * vertex_stride));
      size_t vertex_data_offset = 0;
      for (unsigned long i = 0; i < vertex_count; i++) {
        memcpy(vertex_data + vertex_data_offset,
               position.data + (i * position.stride), position.element_size);
        vertex_data_offset += position.element_size;
        if (has_normals) {
          memcpy(vertex_data + vertex_data_offset,
                 normal.data + (i * normal.stride), normal.element_size);
          vertex_data_offset += normal.element_size;
        }
        if (has_tangents) {
          memcpy(vertex_data + vertex_data_offset,
                 tangent.data + (i * tangent.stride), tangent.element_size);
          vertex_data_offset += tangent.element_size;
        }
        if (has_text_coord_0) {
          memcpy(vertex_data + vertex_data_offset,
                 text_coord_0.data + (i * text_coord_0.stride),
                 text_coord_0.element_size);
          vertex_data_offset += text_coord_0.element_size;
        }
        if (has_text_coord_1) {
          memcpy(vertex_data + vertex_data_offset,
                 text_coord_1.data + (i * text_coord_1.stride),
                 text_coord_1.element_size);
          vertex_data_offset += text_coord_1.element_size;
        }
        if (has_color_0) {
          memcpy(vertex_data + vertex_data_offset,
                 color_0.data + (i * color_0.stride), color_0.element_size);
          vertex_data_offset += color_0.element_size;
        }
        if (has_joints_0) {
          memcpy(vertex_data + vertex_data_offset,
                 joints_0.data + (i * joints_0.stride), joints_0.element_size);
          vertex_data_offset += joints_0.element_size;
        }
        if (has_weights_0) {
          memcpy(vertex_data + vertex_data_offset,
                 weights_0.data + (i * weights_0.stride),
                 weights_0.element_size);
          vertex_data_offset += weights_0.element_size;
        }
      }
      vertex_buffer = context_->CreateBuffer(vertex_count * vertex_stride,
                                             api::BufferUsage::VERTEX_BUFFER,
                                             api::MemoryType::DEVICE_LOCAL);
      vertex_buffer->Update(vertex_data);
      free(vertex_data);
    }
    auto vertex_shader =
        context_->CreateShader(gltf_vertex, "main", api::ShaderType::VERTEX);
    vertex_shader->SetConstant(1, has_normals);
    vertex_shader->SetConstant(2, has_tangents);
    vertex_shader->SetConstant(3, has_text_coord_0);
    vertex_shader->SetConstant(4, has_text_coord_1);
    vertex_shader->SetConstant(5, has_color_0);
    vertex_shader->SetConstant(6, has_joints_0);
    vertex_shader->SetConstant(7, has_weights_0);

    auto fragment_shader = context_->CreateShader(gltf_fragment, "main",
                                                  api::ShaderType::FRAGMENT);

    std::map<unsigned int, std::shared_ptr<api::Texture2D>> textures_mapping{};

    auto material =
        this->model_.materials[static_cast<size_t>(primitive.material)];
    auto pbr = material.pbrMetallicRoughness;

    ubo->material.base_color =
        glm::make_vec4<float>(std::vector<float>(pbr.baseColorFactor.begin(),
                                                 pbr.baseColorFactor.end())
                                  .data());
    if (pbr.baseColorTexture.index > -1) {
      int base_texture_coords_index = pbr.baseColorTexture.texCoord;  // 0 or 1
      textures_mapping[2] =
          textures_[static_cast<size_t>(pbr.baseColorTexture.index)];
      fragment_shader->SetConstant(2, base_texture_coords_index);
    }
    ubo->material.metallic_factor = static_cast<float>(pbr.metallicFactor);
    ubo->material.roughness_factor = static_cast<float>(pbr.roughnessFactor);
    if (pbr.metallicRoughnessTexture.index > -1) {
      int mr_texture_coords_index =
          pbr.metallicRoughnessTexture.texCoord;  // 0 or 1
      textures_mapping[3] =
          textures_[static_cast<size_t>(pbr.metallicRoughnessTexture.index)];
      fragment_shader->SetConstant(3, mr_texture_coords_index);
    }
    if (material.normalTexture.index > -1) {
      int normal_texture_coords_index =
          material.normalTexture.texCoord;  // 0 or 1
      textures_mapping[4] =
          textures_[static_cast<size_t>(material.normalTexture.index)];
      fragment_shader->SetConstant(4, normal_texture_coords_index);
    }
    if (material.occlusionTexture.index > -1) {
      int occlusion_texture_texture_coords_index =
          material.occlusionTexture.texCoord;  // 0 or 1
      textures_mapping[5] =
          textures_[static_cast<size_t>(material.occlusionTexture.index)];
      fragment_shader->SetConstant(5, occlusion_texture_texture_coords_index);
    }
    if (material.emissiveTexture.index > -1) {
      int emissive_texture_coords_index =
          material.emissiveTexture.texCoord;  // 0 or 1
      textures_mapping[6] =
          textures_[static_cast<size_t>(material.emissiveTexture.index)];
      fragment_shader->SetConstant(6, emissive_texture_coords_index);
    }
    ubo->material.emissive_factor =
        glm::make_vec3(std::vector<float>(material.emissiveFactor.begin(),
                                          material.emissiveFactor.end())
                           .data());
    ubo->material.alpha_cutoff = static_cast<float>(material.alphaCutoff);
    ubo->material.double_sided = material.doubleSided;
    auto pipe =
        context_->CreateGraphicsPipeline(vertex_shader, fragment_shader, vbl,
                                         {
                                             GetMode(primitive.mode),
                                             api::CullMode::BACK,
                                             api::FrontFace::CCW,
                                             true,
                                             api::CompareOp::LESS,
                                         });
    pipe->SetVertexBuffer(vertex_buffer);
    pipe->SetIndexBuffer(index_buffer, index_buffer_type);
    for (const auto &entry : textures_mapping) {
      pipe->SetTexture(entry.first, entry.second);
    }
    pipe->UpdateUniformBuffer(1, &ubo->material);
    RenderingUnit unit{pipe, index_count, ubo};
    pipelines.emplace_back(unit);
  }
  return pipelines;
}

void geometry::GltfModel::Render() {
  for (const auto &unit : current_pipelines_) {
    unit.pipeline->UpdateUniformBuffer(0, &unit.ubo->mvp);
    unit.pipeline->Draw(unit.index_count, 0);
  }
}
void geometry::GltfModel::SetViewport(uint32_t width, uint32_t height) {
  for (const auto &unit : current_pipelines_) {
    unit.pipeline->SetViewPort(width, height);
  }
}

void geometry::GltfModel::SetCamera(uint camera_index, glm::mat4 view) {
  auto camera = cameras_[camera_index];
  for (const auto &unit : current_pipelines_) {
    unit.ubo->mvp.projection = camera;
    unit.ubo->mvp.view = view;
  }
}
geometry::ParsedAttribute geometry::GltfModel::ParseAttribute(
    const std::string &attribute_name, int accessor_id) {
  auto accessor = model_.accessors[static_cast<unsigned long>(accessor_id)];
  auto attr_defaults = geometry::kPrimitiveTypes.find(attribute_name);
  auto expected_count = attr_defaults->second.element_count;
  bool matched = false;
  for (auto type : expected_count) {
    if (accessor.type == type) {
      matched = true;
      break;
    }
  }
  CHECK(matched, "unhandled");
  auto expected_data_types = attr_defaults->second.element_type;
  matched = false;
  for (auto type : expected_data_types) {
    if (accessor.componentType == type) {
      matched = true;
      break;
    }
  }
  CHECK(matched, "unhandled");
  auto data_type = GetType(accessor.componentType);
  auto buffer_view =
      model_.bufferViews[static_cast<unsigned long>(accessor.bufferView)];
  CHECK(buffer_view.target == attr_defaults->second.target, "unhandled");
  auto stride_in_current_buffer = buffer_view.byteStride;
  unsigned char *data =
      model_.buffers[static_cast<unsigned long>(buffer_view.buffer)]
          .data.data() +
      buffer_view.byteOffset + accessor.byteOffset;
  auto element_count = GetElementCount(accessor.type);
  auto element_size = element_count * api::GetDataTypeSizeInBytes(data_type);
  if (stride_in_current_buffer == 0) {
    stride_in_current_buffer = element_size;
  }
  return {data,           stride_in_current_buffer,
          accessor.count, element_count,
          data_type,      element_size};
}

std::vector<std::string> geometry::GltfModel::GetScenes() {
  std::vector<std::string> scenes;
  for (size_t i = 0; i < model_.scenes.size(); i++) {
    auto scene = model_.scenes[i];
    if (scene.name.empty()) {
      scenes.push_back(std::to_string(i));
    } else {
      scenes.push_back(scene.name);
    }
  }
  return scenes;
}

int geometry::GltfModel::GetDefaultSceneIndex() const {
  return model_.defaultScene;
}

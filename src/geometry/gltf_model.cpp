#include "geometry/gltf_model.hpp"

#include <spdlog/spdlog.h>

#include "geometry/gltf_model_defaults.hpp"
#include "vulkan/rendering_pipeline.hpp"
#include "vulkan/shader.hpp"
#include "vulkan/utils.hpp"

namespace {
vulkan::DataType GetType(const int type) {
  switch (type) {
    case TINYGLTF_COMPONENT_TYPE_BYTE:
      return vulkan::DataType::BYTE;
    case TINYGLTF_COMPONENT_TYPE_FLOAT:
      return vulkan::DataType::FLOAT;
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
      return vulkan::DataType::UINT_16;
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
      return vulkan::DataType::UINT_32;
    default:
      throw std::runtime_error("unknown type");
  }
}
VkIndexType GetVkIndexType(const vulkan::DataType type) {
  switch (type) {
    case vulkan::DataType::UINT_16:
      return VK_INDEX_TYPE_UINT16;
    case vulkan::DataType::UINT_32:
      return VK_INDEX_TYPE_UINT32;
    default:
      throw std::runtime_error("unsupported index type");
  }
}

unsigned int GetElementCount(const int type) {
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

VkPrimitiveTopology GetMode(const int mode) {
  switch (mode) {
    case TINYGLTF_MODE_POINTS:
      return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
    case TINYGLTF_MODE_LINE:
      return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
    case TINYGLTF_MODE_LINE_STRIP:
      return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
    case TINYGLTF_MODE_TRIANGLES:
      return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    case TINYGLTF_MODE_TRIANGLE_FAN:
      return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
    case TINYGLTF_MODE_TRIANGLE_STRIP:
      return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
    default:
      throw std::runtime_error("unknown mode");
  }
}

VkFilter GetFilter(const int filter) {
  switch (filter) {
    case TINYGLTF_TEXTURE_FILTER_LINEAR:
    case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR:
    case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_NEAREST:
      return VK_FILTER_LINEAR;
    case TINYGLTF_TEXTURE_FILTER_NEAREST:
    case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_NEAREST:
    case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_LINEAR:
      return VK_FILTER_NEAREST;
    case -1:
      return VK_FILTER_LINEAR;
    default:
      throw std::runtime_error("unknown filter");
  }
}

VkSamplerAddressMode GetAddressMode(const int address_mode) {
  switch (address_mode) {
    case TINYGLTF_TEXTURE_WRAP_CLAMP_TO_EDGE:
      return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    case TINYGLTF_TEXTURE_WRAP_MIRRORED_REPEAT:
      return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
    case TINYGLTF_TEXTURE_WRAP_REPEAT:
      return VK_SAMPLER_ADDRESS_MODE_REPEAT;
    default:
      throw std::runtime_error("unknown address mode");
  }
}

VkFormat GetPixelFormat(const tinygltf::Image &image) {
  if (image.pixel_type != TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
    spdlog::error("pixel type must be ubyte");
  }
  if (image.component != 4) {
    spdlog::error("only 4 channel images are supported");
  }
  return VK_FORMAT_R8G8B8A8_UNORM;
}
}  // namespace

std::shared_ptr<geometry::GltfModel> geometry::GltfModel::Create(
    const std::shared_ptr<vulkan::RenderingContext> &context, const std::string &path) {
  return std::shared_ptr<GltfModel>(new GltfModel(context, path));
}

geometry::GltfModel::GltfModel(const std::shared_ptr<vulkan::RenderingContext> &context, const std::string &path)
    : context_(context), command_pool_(context_->CreateCommandPool()) {
  tinygltf::TinyGLTF loader;
  std::string err;
  std::string warn;

  const bool ret = loader.LoadASCIIFromFile(&model_, &err, &warn, path);

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
    auto camera_matrix = glm::mat4(1.0);
    if (camera.type == "perspective") {
      const auto data = camera.perspective;
      camera_matrix = glm::perspective(data.yfov, data.aspectRatio, data.znear, data.zfar);
    } else {
      const auto data = camera.orthographic;
      camera_matrix[0][0] = static_cast<float>(1.0F / data.xmag);
      camera_matrix[1][1] = static_cast<float>(1.0F / data.ymag);
      camera_matrix[2][2] = static_cast<float>(2.0F / (data.znear - data.zfar));
      camera_matrix[3][2] = static_cast<float>((data.zfar + data.znear) / (data.znear - data.zfar));
    }
    cameras_.emplace_back(camera_matrix);
  }
  for (const auto &kSampler : model_.samplers) {
    samplers_.emplace_back(vulkan::Sampler::Create(context_, GetFilter(kSampler.magFilter),
                                                   GetFilter(kSampler.minFilter), GetAddressMode(kSampler.wrapS),
                                                   GetAddressMode(kSampler.wrapT)));
  }
  for (const auto &kImage : model_.images) {
    auto vulkan_image = vulkan::Image::Create(context_, kImage.width, kImage.height, GetPixelFormat(kImage),
                                              VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, false,
                                              VK_SAMPLE_COUNT_1_BIT);
    auto staging_buffer =
        vulkan::Buffer::Create(context, vulkan_image->GetSizeInBytes(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, true);
    std::memcpy(staging_buffer->Map(), kImage.image.data(), staging_buffer->GetSizeInBytes());
    staging_buffer->Unmap();
    auto *command_buffer = context_->CreateCommandBuffer(command_pool_);
    VkCommandBufferBeginInfo command_buffer_begin_info{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        .pInheritanceInfo = nullptr,
    };
    VK_CALL(vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info));
    vulkan_image->CopyFrom(command_buffer, staging_buffer);
    vulkan_image->TransferTo(command_buffer, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    VK_CALL(vkEndCommandBuffer(command_buffer));
    auto *fence = context->CreateFence();
    context->SubmitCommandBuffer(command_buffer, VK_NULL_HANDLE, VK_PIPELINE_STAGE_2_NONE, VK_NULL_HANDLE, fence);
    context->WaitForFence(fence);
    context->DestroyFence(fence);
    context->FreeCommandBuffer(command_pool_, command_buffer);
    images_.emplace_back(vulkan_image);
  }
  for (const auto &kTexture : model_.textures) {
    auto image_view = vulkan::ImageView::Create(context_, images_[kTexture.source]);
    textures_.emplace_back(image_view, samplers_[kTexture.sampler]);
  }
}

void geometry::GltfModel::LoadScene(const size_t scene_index) {
  if (model_.scenes.empty()) {
    spdlog::error("model must have at least 1 scene");
  }
  if (scene_index >= model_.scenes.size()) {
    throw std::runtime_error("scene with given index does not exists");
  }

  context_->WaitForQueueIdle();
  current_pipelines_.clear();
  auto default_scene = model_.scenes[scene_index];
  for (const auto &kNode : default_scene.nodes) {
    LoadNode(model_.nodes[static_cast<size_t>(kNode)]);
  }
}

void geometry::GltfModel::LoadNode(const tinygltf::Node &node, const glm::mat4 &parent_transform) {
  auto matrix = glm::mat4(1.0F);
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
    auto pipes = LoadMesh(model_.meshes[static_cast<size_t>(node.mesh)], matrix);
    current_pipelines_.insert(current_pipelines_.end(), pipes.begin(), pipes.end());
  }
  if (node.camera != -1) {
    SetCamera(node.camera, glm::inverse(matrix));
  }
  for (const auto &child : node.children) {
    LoadNode(model_.nodes[static_cast<size_t>(child)], matrix);
  }
}

std::vector<geometry::RenderingUnit> geometry::GltfModel::LoadMesh(tinygltf::Mesh &mesh, glm::mat4 model_matrix) {
  std::vector<RenderingUnit> rendering_units{};
  for (const auto &kPrimitive : mesh.primitives) {
    PrimitiveUbo ubo;
    ubo.mvp.model = model_matrix;
    ubo.mvp.normal = inverseTranspose(model_matrix);
    std::shared_ptr<vulkan::Buffer> index_buffer = nullptr;
    VkIndexType index_buffer_type = VK_INDEX_TYPE_UINT16;
    size_t index_count = 0;
    std::shared_ptr<vulkan::Buffer> vertex_buffer = nullptr;
    auto vbl = vulkan::VertexBufferLayout();
    bool has_normals = false;
    bool has_tangents = false;
    bool has_text_coord_0 = false;
    bool has_text_coord_1 = false;
    bool has_color_0 = false;
    bool has_joints_0 = false;
    bool has_weights_0 = false;

    {  // index buffer
      if (kPrimitive.indices == -1) {
        throw std::runtime_error("unhandled");
      }
      auto index = ParseAttribute("INDICES", kPrimitive.indices);
      auto data = std::vector<char>(index.instance_count * index.element_size);
      size_t offset = 0;
      for (uint64_t i = 0; i < index.instance_count; i++) {
        memcpy(data.data() + offset, index.data + (i * index.stride), index.element_size);
        offset += index.element_size;
      }
      index_buffer = vulkan::Buffer::Create(
          context_, static_cast<uint32_t>(index.instance_count) * GetDataTypeSizeInBytes(index.element_type),
          VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, false);

      index_buffer_type = GetVkIndexType(index.element_type);
      index_count = index.instance_count;

      auto staging_buffer =
          vulkan::Buffer::Create(context_, index_buffer->GetSizeInBytes(), VK_IMAGE_USAGE_TRANSFER_SRC_BIT, true);
      std::memcpy(staging_buffer->Map(), data.data(), staging_buffer->GetSizeInBytes());
      staging_buffer->Unmap();
      auto *command_buffer = context_->CreateCommandBuffer(command_pool_);
      VkCommandBufferBeginInfo command_buffer_begin_info{
          .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
          .pNext = nullptr,
          .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
          .pInheritanceInfo = nullptr,
      };
      VK_CALL(vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info));
      index_buffer->CopyFrom(command_buffer, staging_buffer, staging_buffer->GetSizeInBytes());
      VK_CALL(vkEndCommandBuffer(command_buffer));
      auto *fence = context_->CreateFence();
      context_->SubmitCommandBuffer(command_buffer, VK_NULL_HANDLE, VK_PIPELINE_STAGE_2_NONE, VK_NULL_HANDLE, fence);
      context_->WaitForFence(fence);
      context_->DestroyFence(fence);
      context_->FreeCommandBuffer(command_pool_, command_buffer);
    }
    {
      auto attributes = kPrimitive.attributes;
      ParsedAttribute position;
      ParsedAttribute normal{};
      ParsedAttribute tangent{};
      ParsedAttribute text_coord_0{};
      ParsedAttribute text_coord_1{};
      ParsedAttribute color_0{};
      ParsedAttribute joints_0{};
      ParsedAttribute weights_0{};

      auto position_attr = attributes.find("POSITION");
      if (position_attr == attributes.end()) {
        throw std::runtime_error("must contain position attribute");
      }
      position = ParseAttribute("POSITION", position_attr->second);
      auto vertex_count = position.instance_count;
      vbl.Push({.binding_index = 0, .type = position.element_type, .count = position.element_count});

      if (attributes.contains("NORMAL")) {
        normal = ParseAttribute("NORMAL", attributes.find("NORMAL")->second);

        vbl.Push({.binding_index = 1, .type = normal.element_type, .count = normal.element_count});
        has_normals = true;
      }
      if (attributes.contains("TANGENT")) {
        tangent = ParseAttribute("TANGENT", attributes.find("TANGENT")->second);
        if (normal.instance_count == vertex_count) {
          throw std::runtime_error("unhandled");
        }
        vbl.Push({.binding_index = 2, .type = tangent.element_type, .count = tangent.element_count});
        has_tangents = true;
      }
      if (attributes.contains("TEXCOORD_0")) {
        text_coord_0 = ParseAttribute("TEXCOORD_0", attributes.find("TEXCOORD_0")->second);
        if (text_coord_0.instance_count != vertex_count) {
          throw std::runtime_error("unhandled");
        }
        vbl.Push({.binding_index = 3, .type = text_coord_0.element_type, .count = text_coord_0.element_count});
        has_text_coord_0 = true;
      }
      if (attributes.contains("TEXCOORD_1")) {
        text_coord_1 = ParseAttribute("TEXCOORD_1", attributes.find("TEXCOORD_1")->second);
        if (text_coord_1.instance_count != vertex_count) {
          throw std::runtime_error("unhandled");
        }
        vbl.Push({.binding_index = 4, .type = text_coord_1.element_type, .count = text_coord_1.element_count});
        has_text_coord_1 = true;
      }
      if (attributes.contains("COLOR_0")) {
        color_0 = ParseAttribute("TEXCOORD_1", attributes.find("TEXCOORD_1")->second);
        if (color_0.instance_count != vertex_count) {
          throw std::runtime_error("unhandled");
        }
        vbl.Push({.binding_index = 5, .type = color_0.element_type, .count = color_0.element_count});
        has_color_0 = true;
      }
      if (attributes.contains("JOINTS_0")) {
        joints_0 = ParseAttribute("JOINTS_0", attributes.find("JOINTS_0")->second);
        if (joints_0.instance_count != vertex_count) {
          throw std::runtime_error("unhandled");
        }
        vbl.Push({.binding_index = 6, .type = joints_0.element_type, .count = joints_0.element_count});
        has_joints_0 = true;
      }
      if (attributes.contains("WEIGHTS_0")) {
        weights_0 = ParseAttribute("WEIGHTS_0", attributes.find("WEIGHTS_0")->second);
        if (weights_0.instance_count != vertex_count) {
          throw std::runtime_error("unhandled");
        }
        vbl.Push({.binding_index = 7, .type = weights_0.element_type, .count = weights_0.element_count});
        has_weights_0 = true;
      }
      auto vertex_stride = vbl.GetElementSize();
      std::vector<char> vertex_data = std::vector<char>(vertex_count * vertex_stride);
      size_t vertex_data_offset = 0;
      for (uint64_t i = 0; i < vertex_count; i++) {
        memcpy(vertex_data.data() + vertex_data_offset, position.data + (i * position.stride), position.element_size);
        vertex_data_offset += position.element_size;
        if (has_normals) {
          memcpy(vertex_data.data() + vertex_data_offset, normal.data + (i * normal.stride), normal.element_size);
          vertex_data_offset += normal.element_size;
        }
        if (has_tangents) {
          memcpy(vertex_data.data() + vertex_data_offset, tangent.data + (i * tangent.stride), tangent.element_size);
          vertex_data_offset += tangent.element_size;
        }
        if (has_text_coord_0) {
          memcpy(vertex_data.data() + vertex_data_offset, text_coord_0.data + (i * text_coord_0.stride),
                 text_coord_0.element_size);
          vertex_data_offset += text_coord_0.element_size;
        }
        if (has_text_coord_1) {
          memcpy(vertex_data.data() + vertex_data_offset, text_coord_1.data + (i * text_coord_1.stride),
                 text_coord_1.element_size);
          vertex_data_offset += text_coord_1.element_size;
        }
        if (has_color_0) {
          memcpy(vertex_data.data() + vertex_data_offset, color_0.data + (i * color_0.stride), color_0.element_size);
          vertex_data_offset += color_0.element_size;
        }
        if (has_joints_0) {
          memcpy(vertex_data.data() + vertex_data_offset, joints_0.data + (i * joints_0.stride), joints_0.element_size);
          vertex_data_offset += joints_0.element_size;
        }
        if (has_weights_0) {
          memcpy(vertex_data.data() + vertex_data_offset, weights_0.data + (i * weights_0.stride),
                 weights_0.element_size);
          vertex_data_offset += weights_0.element_size;
        }
      }
      vertex_buffer =
          vulkan::Buffer::Create(context_, vertex_count * vertex_stride,
                                 VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, false);

      auto staging_buffer =
          vulkan::Buffer::Create(context_, vertex_buffer->GetSizeInBytes(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, true);
      std::memcpy(staging_buffer->Map(), vertex_data.data(), staging_buffer->GetSizeInBytes());
      staging_buffer->Unmap();
      auto *command_buffer = context_->CreateCommandBuffer(command_pool_);
      VkCommandBufferBeginInfo command_buffer_begin_info{
          .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
          .pNext = nullptr,
          .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
          .pInheritanceInfo = nullptr,
      };
      VK_CALL(vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info));
      vertex_buffer->CopyFrom(command_buffer, staging_buffer, staging_buffer->GetSizeInBytes());
      VK_CALL(vkEndCommandBuffer(command_buffer));
      auto *fence = context_->CreateFence();
      context_->SubmitCommandBuffer(command_buffer, VK_NULL_HANDLE, VK_PIPELINE_STAGE_2_NONE, VK_NULL_HANDLE, fence);
      context_->WaitForFence(fence);
      context_->DestroyFence(fence);
      context_->FreeCommandBuffer(command_pool_, command_buffer);
    }
    auto vertex_shader = vulkan::Shader::Create(context_,
                                                {
#include "gltf_vertex.spv"

                                                },
                                                "main");
    vertex_shader->SetConstant(1, has_normals);
    vertex_shader->SetConstant(2, has_tangents);
    vertex_shader->SetConstant(3, has_text_coord_0);
    vertex_shader->SetConstant(4, has_text_coord_1);
    vertex_shader->SetConstant(5, has_color_0);
    vertex_shader->SetConstant(6, has_joints_0);
    vertex_shader->SetConstant(7, has_weights_0);

    auto fragment_shader = vulkan::Shader::Create(context_,
                                                  {
#include "gltf_fragment.spv"

                                                  },
                                                  "main");

    std::map<unsigned int, CombinedImageSampler> textures_mapping{};

    auto material = this->model_.materials[static_cast<size_t>(kPrimitive.material)];
    auto pbr = material.pbrMetallicRoughness;

    ubo.material.base_color =
        glm::make_vec4<float>(std::vector<float>(pbr.baseColorFactor.begin(), pbr.baseColorFactor.end()).data());
    if (pbr.baseColorTexture.index > -1) {
      int base_texture_coords_index = pbr.baseColorTexture.texCoord;  // 0 or 1
      textures_mapping[2] = textures_[static_cast<size_t>(pbr.baseColorTexture.index)];
      fragment_shader->SetConstant(2, base_texture_coords_index);
    }
    ubo.material.metallic_factor = static_cast<float>(pbr.metallicFactor);
    ubo.material.roughness_factor = static_cast<float>(pbr.roughnessFactor);
    if (pbr.metallicRoughnessTexture.index > -1) {
      int mr_texture_coords_index = pbr.metallicRoughnessTexture.texCoord;  // 0 or 1
      textures_mapping[3] = textures_[static_cast<size_t>(pbr.metallicRoughnessTexture.index)];
      fragment_shader->SetConstant(3, mr_texture_coords_index);
    }
    if (material.normalTexture.index > -1) {
      int normal_texture_coords_index = material.normalTexture.texCoord;  // 0 or 1
      textures_mapping[4] = textures_[static_cast<size_t>(material.normalTexture.index)];
      fragment_shader->SetConstant(4, normal_texture_coords_index);
    }
    if (material.occlusionTexture.index > -1) {
      int occlusion_texture_texture_coords_index = material.occlusionTexture.texCoord;  // 0 or 1
      textures_mapping[5] = textures_[static_cast<size_t>(material.occlusionTexture.index)];
      fragment_shader->SetConstant(5, occlusion_texture_texture_coords_index);
    }
    if (material.emissiveTexture.index > -1) {
      int emissive_texture_coords_index = material.emissiveTexture.texCoord;  // 0 or 1
      textures_mapping[6] = textures_[static_cast<size_t>(material.emissiveTexture.index)];
      fragment_shader->SetConstant(6, emissive_texture_coords_index);
    }
    ubo.material.emissive_factor =
        glm::make_vec3(std::vector<float>(material.emissiveFactor.begin(), material.emissiveFactor.end()).data());
    ubo.material.alpha_cutoff = static_cast<float>(material.alphaCutoff);
    ubo.material.double_sided = material.doubleSided;

    auto rendering_unit = RenderingUnit(context_, vertex_shader, fragment_shader, vbl, vertex_buffer, index_count,
                                        index_buffer_type, index_buffer, GetMode(kPrimitive.mode), textures_mapping);
    rendering_unit.ubo_data_ = ubo;
    rendering_unit.UpdateMvp();
    if (render_pass_ != nullptr) {
      rendering_unit.RenderPassChanged(context_, render_pass_, descriptor_set_count_, sample_count_);
    }
    rendering_units.emplace_back(rendering_unit);
  }
  return rendering_units;
}

void geometry::GltfModel::Render(const VkCommandBuffer &command_buffer, const uint16_t descriptor_set_index) const {
  for (const auto &kRenderingUint : current_pipelines_) {
    kRenderingUint.Draw(command_buffer, descriptor_set_index);
  }
}

void geometry::GltfModel::SetCamera(const size_t camera_index, const glm::mat4 &view) {
  const auto camera = cameras_[camera_index];
  for (auto &k_unit : current_pipelines_) {
    k_unit.ubo_data_.mvp.projection = camera;
    k_unit.ubo_data_.mvp.view = view;
    k_unit.UpdateMvp();
  }
}
geometry::ParsedAttribute geometry::GltfModel::ParseAttribute(const std::string &attribute_name,
                                                              const int accessor_id) {
  const auto accessor = model_.accessors[static_cast<uint64_t>(accessor_id)];
  const auto attr_defaults = kPrimitiveTypes.find(attribute_name);
  const auto expected_count = attr_defaults->second.element_count;
  bool matched = false;
  for (const auto type : expected_count) {
    if (accessor.type == type) {
      matched = true;
      break;
    }
  }
  if (!matched) {
    throw std::runtime_error("unhandled");
  }
  const auto expected_data_types = attr_defaults->second.element_type;
  matched = false;
  for (const auto type : expected_data_types) {
    if (accessor.componentType == type) {
      matched = true;
      break;
    }
  }
  if (!matched) {
    throw std::runtime_error("unhandled");
  }
  const auto data_type = GetType(accessor.componentType);
  const auto buffer_view = model_.bufferViews[static_cast<uint64_t>(accessor.bufferView)];
  if (buffer_view.target != attr_defaults->second.target) {
    throw std::runtime_error("unhandled");
  }
  auto stride_in_current_buffer = buffer_view.byteStride;
  unsigned char *data = model_.buffers[static_cast<uint64_t>(buffer_view.buffer)].data.data() + buffer_view.byteOffset +
                        accessor.byteOffset;
  const auto element_count = GetElementCount(accessor.type);
  const auto element_size = element_count * GetDataTypeSizeInBytes(data_type);
  if (stride_in_current_buffer == 0) {
    stride_in_current_buffer = element_size;
  }
  return {.data = data,
          .stride = stride_in_current_buffer,
          .instance_count = accessor.count,
          .element_count = element_count,
          .element_type = data_type,
          .element_size = element_size};
}

std::vector<std::string> geometry::GltfModel::GetScenes() const {
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

int geometry::GltfModel::GetDefaultSceneIndex() const { return model_.defaultScene; }

geometry::GltfModel::~GltfModel() { context_->DestroyCommandPool(command_pool_); }

void geometry::GltfModel::OnRenderPassChanged(const std::shared_ptr<vulkan::RenderPass> &render_pass,
                                              const size_t descriptor_set_count,
                                              const VkSampleCountFlagBits sample_count) {
  render_pass_ = render_pass;
  descriptor_set_count_ = descriptor_set_count;
  sample_count_ = sample_count;
  for (auto &k_rendering_uint : current_pipelines_) {
    k_rendering_uint.RenderPassChanged(context_, render_pass, descriptor_set_count, sample_count);
  }
}

void geometry::RenderingUnit::RenderPassChanged(const std::shared_ptr<vulkan::RenderingContext> &context,
                                                const std::shared_ptr<vulkan::RenderPass> &render_pass,
                                                const size_t descriptor_set_count,
                                                const VkSampleCountFlagBits sample_count) {
  pipeline_ = vulkan::RenderingPipeline::Create(context, render_pass, vertex_shader_, fragment_shader_, vbl_,
                                                {
                                                    .draw_mode = draw_mode_,
                                                    .cull_mode = VK_CULL_MODE_BACK_BIT,
                                                    .front_face = VK_FRONT_FACE_COUNTER_CLOCKWISE,
                                                    .enable_depth_test = VK_TRUE,
                                                    .depth_function = VK_COMPARE_OP_LESS,
                                                    .sample_count = sample_count,
                                                },
                                                descriptor_set_count);

  pipeline_->SetVertexBuffer(0, vertex_buffer_);
  pipeline_->SetIndexBuffer(index_buffer_, index_type_);

  for (size_t descriptor_index = 0; descriptor_index < descriptor_set_count; descriptor_index++) {
    for (const auto &kEntry : textures_) {
      pipeline_->SetImageView(kEntry.first, descriptor_index, kEntry.second.first, kEntry.second.second);
    }
    pipeline_->SetUniformBuffer(1, descriptor_index, material_uniform_buffer_);
    pipeline_->SetUniformBuffer(0, descriptor_index, mvp_uniform_buffer_);
  }
}

geometry::RenderingUnit::RenderingUnit(
    const std::shared_ptr<vulkan::RenderingContext> &context, const std::shared_ptr<vulkan::Shader> &vertex_shader,
    const std::shared_ptr<vulkan::Shader> &fragment_shader, vulkan::VertexBufferLayout vbl,
    const std::shared_ptr<vulkan::Buffer> &vertex_buffer, const size_t index_count, const VkIndexType index_type,
    const std::shared_ptr<vulkan::Buffer> &index_buffer, const VkPrimitiveTopology draw_mode,
    const std::map<unsigned int, CombinedImageSampler> &textures)
    : vertex_shader_(vertex_shader),
      fragment_shader_(fragment_shader),
      vbl_(std::move(vbl)),
      vertex_buffer_(vertex_buffer),
      index_count_(index_count),
      index_type_(index_type),
      index_buffer_(index_buffer),
      draw_mode_(draw_mode),
      textures_(textures) {
  mvp_uniform_buffer_ = vulkan::Buffer::Create(context, sizeof(MVP), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, true);
  material_uniform_buffer_ =
      vulkan::Buffer::Create(context, sizeof(Material), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, true);
}

void geometry::RenderingUnit::Draw(const VkCommandBuffer &command_buffer, const uint16_t descriptor_set_index) const {
  pipeline_->DrawIndexed(command_buffer, index_count_, descriptor_set_index);
}

void geometry::RenderingUnit::UpdateMvp() const {
  std::memcpy(mvp_uniform_buffer_->Map(), &ubo_data_.mvp, mvp_uniform_buffer_->GetSizeInBytes());
  mvp_uniform_buffer_->Unmap();
}

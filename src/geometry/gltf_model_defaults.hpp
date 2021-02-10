//
// Created by artyomd on 1/12/21.
//
#pragma once

namespace geometry {
struct ExpectedValues {
  std::vector<int> element_count;
  std::vector<int> element_type;
  int target;
};
static const std::map<std::string, ExpectedValues> kPrimitiveTypes{
    {
        "INDICES",
        {
            .element_count = {TINYGLTF_TYPE_SCALAR},
            .element_type = {TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE,
                             TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT,
                             TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT},
            .target = TINYGLTF_TARGET_ELEMENT_ARRAY_BUFFER
        }
    },
    {
        "POSITION",
        {
            .element_count = {TINYGLTF_TYPE_VEC3},
            .element_type = {TINYGLTF_COMPONENT_TYPE_FLOAT},
            .target = TINYGLTF_TARGET_ARRAY_BUFFER
        }
    },
    {
        "NORMAL",
        {
            .element_count = {TINYGLTF_TYPE_VEC3},
            .element_type = {TINYGLTF_COMPONENT_TYPE_FLOAT},
            .target = TINYGLTF_TARGET_ARRAY_BUFFER
        }
    },
    {
        "TANGENT",
        {
            .element_count = {TINYGLTF_TYPE_VEC4},
            .element_type = {TINYGLTF_COMPONENT_TYPE_FLOAT},
            .target = TINYGLTF_TARGET_ARRAY_BUFFER
        }
    },
    {
        "TEXCOORD_0",
        {
            .element_count = {TINYGLTF_TYPE_VEC2},
            .element_type = {
                TINYGLTF_COMPONENT_TYPE_FLOAT,
                TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE,
                TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT
            },
            .target = TINYGLTF_TARGET_ARRAY_BUFFER
        }
    },
    {
        "TEXCOORD_1",
        {
            .element_count = {TINYGLTF_TYPE_VEC2},
            .element_type = {
                TINYGLTF_COMPONENT_TYPE_FLOAT,
                TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE,
                TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT
            },
            .target = TINYGLTF_TARGET_ARRAY_BUFFER
        }
    },
    {
        "COLOR_0",
        {
            .element_count = {
                TINYGLTF_TYPE_VEC3,
                TINYGLTF_TYPE_VEC4
            },
            .element_type = {
                TINYGLTF_COMPONENT_TYPE_FLOAT,
                TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE,
                TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT
            },
            .target = TINYGLTF_TARGET_ARRAY_BUFFER
        }
    },
    {
        "JOINTS_0",
        {
            .element_count = {TINYGLTF_TYPE_VEC4},
            .element_type = {
                TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE,
                TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT
            },
            .target = TINYGLTF_TARGET_ARRAY_BUFFER
        }
    },
    {
        "WEIGHTS_0",
        {
            .element_count =  {TINYGLTF_TYPE_VEC4},
            .element_type = {
                TINYGLTF_COMPONENT_TYPE_FLOAT,
                TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE,
                TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT
            },
            .target = TINYGLTF_TARGET_ARRAY_BUFFER
        }
    }
};
}

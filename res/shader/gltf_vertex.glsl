#version 460
#pragma shader_stage(vertex)
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 position;
layout(location = 0) out vec3 v_position;

layout(location = 1) in vec3 normal;
layout(constant_id = 1) const bool has_normal = false;

layout(location = 2) in vec2 tangent;
layout(constant_id = 2) const bool has_tangent = false;

layout(location = 3) in vec2 textCoord_0;
layout(constant_id = 3) const bool has_textCoord_0 = false;
layout(location = 3) out vec2 v_texCoord_0;

layout(location = 4) in vec2 textCoord_1;
layout(constant_id = 4) const bool has_textCoord_1 = false;
layout(location = 4) out vec2 v_texCoord_1;

layout(location = 5) in vec2 color_0;
layout(constant_id = 5) const bool has_color_0 = false;

layout(location = 6) in vec2 joints_0;
layout(constant_id = 6) const bool has_joints_0 = false;

layout(location = 7) in vec2 weights_0;
layout(constant_id = 7) const bool has_weights_0 = false;


layout(binding = 0, std140) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
};

void main() {
    vec4 pos = model * position;
    v_position = vec3(pos.xyz) / pos.w;
    gl_Position = proj * view *pos;
    v_texCoord_0 = vec2(0.0, 0.0);
    v_texCoord_1 = vec2(0.0, 0.0);
    if (has_textCoord_0){
        v_texCoord_0 = textCoord_0;
    }
    if (has_textCoord_1){
        v_texCoord_1 = textCoord_1;
    }
}

#version 460
#pragma shader_stage(vertex)
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 a_position;
layout(location = 0) out vec3 v_position;

layout(location = 1) in vec3 a_normal;
layout(constant_id = 1) const bool has_normal = false;
layout(location = 1) out vec3 v_normal;

layout(location = 2) in vec4 a_tangent;
layout(constant_id = 2) const bool has_tangent = false;
layout(location = 2) out mat3 v_TBN;

layout(location = 3) in vec2 a_textCoord_0;
layout(constant_id = 3) const bool has_textCoord_0 = false;
layout(location = 5) out vec2 v_texCoord_0;

layout(location = 4) in vec2 a_textCoord_1;
layout(constant_id = 4) const bool has_textCoord_1 = false;
layout(location = 6) out vec2 v_texCoord_1;

layout(location = 5) in vec4 a_color_0;
layout(constant_id = 5) const bool has_color_0 = false;
layout(location = 7) out vec4 v_color_0;

layout(location = 6) in vec2 a_joints_0;
layout(constant_id = 6) const bool has_joints_0 = false;

layout(location = 7) in vec2 a_weights_0;
layout(constant_id = 7) const bool has_weights_0 = false;


layout(binding = 0, std140) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    mat4 normal;
} ubo;

vec4 getPosition()
{
    return a_position;
}
vec3 getNormal()
{
    return normalize(a_normal);
}

vec3 getTangent()
{
    vec3 tangent = a_tangent.xyz;
    return normalize(tangent);
}

void main() {
    vec4 pos = ubo.model * getPosition();
    v_position = vec3(pos.xyz) / pos.w;

    if (has_normal){
        if (has_tangent){
            vec3 tangent = getTangent();
            vec3 normalW = normalize(vec3(ubo.normal * vec4(getNormal(), 0.0)));
            vec3 tangentW = normalize(vec3(ubo.model * vec4(tangent, 0.0)));
            vec3 bitangentW = cross(normalW, tangentW) * a_tangent.w;
            v_TBN = mat3(tangentW, bitangentW, normalW);
        } else {
            v_normal = normalize(vec3(ubo.normal * vec4(getNormal(), 0.0)));
        }
    }
    if (has_textCoord_0){
        v_texCoord_0 = a_textCoord_0;
    } else {
        v_texCoord_0 = vec2(0.0, 0.0);
    }
    if (has_textCoord_1){
        v_texCoord_1 = a_textCoord_1;
    } else {
        v_texCoord_1 = vec2(0.0, 0.0);
    }

    if(has_color_0){
        v_color_0 = a_color_0;
    }else{
        v_color_0 = vec4(0.0);
    }

    gl_Position = ubo.proj* ubo.view * pos;
}

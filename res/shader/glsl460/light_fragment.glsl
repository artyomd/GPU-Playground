#version 460
#pragma shader_stage(fragment)
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

layout(location = 0) out vec4 color;

layout(binding = 1, std140) uniform ViewBuffer {
  vec4 eyePos;
  vec4 lightPos;
};

vec3 cSurface = vec3(0.5, 0.5, 0.5);
vec3 cCool = vec3(0.0, 0.0, 0.55) + 0.25 * cSurface;
vec3 cWarm = vec3(0.3, 0.3, 0.0) + 0.25 * cSurface;
vec3 cHighlight = vec3(1, 1, 1);

void main() {
  vec3 n = normalize(normal);
  vec3 v = normalize(eyePos.xyz - position);
  vec3 l = normalize(lightPos.xyz - position);
  vec3 r = 2.f * dot(n, l) * n - l;
  float t = (dot(n, l) + 1.0f) / 2.0f;
  float s = clamp(100.0 * dot(r, v) - 97.0, 0.0, 1.0);
  color.rgb = s * cHighlight + (1.0f - s) * (t * cWarm + (1.f - t) * cCool);
  color.a = 1.0;
}

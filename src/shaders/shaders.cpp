//
// Created by artyomd on 5/29/21.
//
#include <string>

std::string color_fragment_shader;
std::string default_color_fragment_shader;
std::string default_color_vertex_shader;
std::string default_empty_vertex_shader;
std::string default_mvp_color_vertex_shader;
std::string gltf_fragment;
std::string gltf_vertex;
std::string obj_fragment;
std::string obj_vertex;
std::string raymarched_reflections;
std::string seascape_fragment_shader;
std::string shaping_function_fragment_shader;
std::string squares_fragment_shader;
std::string start_nest_fragment_shader;
std::string texture2d_fragment;
std::string texture2d_vertex;

#define STRINGIFY(_name_) #_name_
#define SHADER(_name_)  STRINGIFY(_name_.spv)
#define SHADER_TO_STRING(shader) std::string(reinterpret_cast<const char*>(shader), static_cast<size_t>(sizeof(shader)/sizeof(shader[0])))

void LoadShaders() {
  const unsigned char color_fragment[] = {
#include SHADER(color_fragment_shader)
  };
  color_fragment_shader = SHADER_TO_STRING(color_fragment);

  const unsigned char default_color_fragment[] = {
#include SHADER(default_color_fragment_shader)
  };
  default_color_fragment_shader = SHADER_TO_STRING(default_color_fragment);

  const unsigned char default_color_vertex[] = {
#include SHADER(default_color_vertex_shader)
  };
  default_color_vertex_shader = SHADER_TO_STRING(default_color_vertex);

  const unsigned char default_empty_vertex[] = {
#include SHADER(default_empty_vertex_shader)
  };
  default_empty_vertex_shader = SHADER_TO_STRING(default_empty_vertex);

  const unsigned char default_mvp_color_vertex[] = {
#include SHADER(default_mvp_color_vertex_shader)
  };
  default_mvp_color_vertex_shader = SHADER_TO_STRING(default_mvp_color_vertex);

  const unsigned char gltf_fragment_source[] = {
#include SHADER(gltf_fragment)
  };
  gltf_fragment = SHADER_TO_STRING(gltf_fragment_source);

  const unsigned char gltf_vertex_source[] = {
#include SHADER(gltf_vertex)
  };
  gltf_vertex = SHADER_TO_STRING(gltf_vertex_source);

  const unsigned char obj_fragment_source[] = {
#include SHADER(obj_fragment)
  };
  obj_fragment = SHADER_TO_STRING(obj_fragment_source);

  const unsigned char obj_vertex_source[] = {
#include SHADER(obj_vertex)
  };
  obj_vertex = SHADER_TO_STRING(obj_vertex_source);

  const unsigned char raymarched_reflections_source[] = {
#include SHADER(raymarched_reflections)
  };
  raymarched_reflections = SHADER_TO_STRING(raymarched_reflections_source);

  const unsigned char seascape_fragment[] = {
#include SHADER(seascape_fragment_shader)
  };
  seascape_fragment_shader = SHADER_TO_STRING(seascape_fragment);

  const unsigned char shaping_function_fragment[] = {
#include SHADER(shaping_function_fragment_shader)
  };
  shaping_function_fragment_shader = SHADER_TO_STRING(shaping_function_fragment);

  const unsigned char squares_fragment[] = {
#include SHADER(squares_fragment_shader)
  };
  squares_fragment_shader = SHADER_TO_STRING(squares_fragment);

  const unsigned char start_nest_fragment[] = {
#include SHADER(start_nest_fragment_shader)
  };
  start_nest_fragment_shader = SHADER_TO_STRING(start_nest_fragment);

  const unsigned char texture2d_fragment_source[] = {
#include SHADER(texture2d_fragment)
  };
  texture2d_fragment = SHADER_TO_STRING(texture2d_fragment_source);

  const unsigned char texture2d_vertex_source[] = {
#include SHADER(texture2d_vertex)
  };
  texture2d_vertex = SHADER_TO_STRING(texture2d_vertex_source);
}
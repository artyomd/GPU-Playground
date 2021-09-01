//
// Created by artyomd on 5/29/21.
//

#pragma once

#include <string>

extern std::string color_fragment_shader;
extern std::string default_color_fragment_shader;
extern std::string default_color_vertex_shader;
extern std::string default_empty_vertex_shader;
extern std::string default_mvp_color_vertex_shader;
extern std::string gltf_fragment;
extern std::string gltf_vertex;
extern std::string light_fragment;
extern std::string light_vertex;
extern std::string raymarched_reflections;
extern std::string seascape_fragment_shader;
extern std::string shaping_function_fragment_shader;
extern std::string squares_fragment_shader;
extern std::string start_nest_fragment_shader;
extern std::string texture2d_fragment;
extern std::string texture2d_vertex;

void LoadShaders();

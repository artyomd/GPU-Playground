set(GLSL_FILES
        color_fragment_shader.glsl
        default_color_fragment_shader.glsl
        default_color_vertex_shader.glsl
        default_empty_vertex_shader.glsl
        default_mvp_color_vertex_shader.glsl
        gltf_fragment.glsl
        gltf_vertex.glsl
        hsb_fragment.glsl
        lighting_fragment.glsl
        lighting_vertex.glsl
        shaping_function_fragment_shader.glsl
        squares_fragment_shader.glsl
        texture2d_fragment.glsl
        texture2d_vertex.glsl
)

list(TRANSFORM GLSL_FILES PREPEND "${CMAKE_CURRENT_LIST_DIR}/")

set(SHADERS_DEBUG OFF)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(SHADERS_DEBUG ON)
elseif (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set(SHADERS_DEBUG ON)
endif ()

add_spirv_library(LIBRARY_NAME shaders
        DEBUG ${SHADERS_DEBUG}
        WERROR ${SHADERS_DEBUG}
        INPUT_GLSL_FILE ${GLSL_FILES})

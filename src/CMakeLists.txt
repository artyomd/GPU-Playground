add_subdirectory(shaders)

add_executable(playground
        application/application.cpp
        geometry/geometry_item.cpp
        geometry/gltf_model.cpp
        geometry/plane_2d.cpp
        geometry/quad.cpp
        geometry/spiral_sphere.cpp
        geometry/stacked_sphere.cpp
        geometry/triangle.cpp
        impl/stb_impl.cpp
        impl/tiny_gltf.cpp
        impl/vma_impl.cpp
        impl/volk_impl.cpp
        main.cpp
        renderables/imgui_wrapper.cpp
        renderables/menu/default_menu.cpp
        renderables/menu/menu_manager.cpp
        renderables/model/cube.cpp
        renderables/model/gltf_model.cpp
        renderables/model/lighting.cpp
        renderables/model/model.cpp
        renderables/model/obj_model.cpp
        renderables/model/spiral_sphere.cpp
        renderables/model/stacked_sphere.cpp
        renderables/model/texture_2d.cpp
        renderables/model/triangle.cpp
        renderables/shader.cpp
        vulkan/buffer.cpp
        vulkan/framebuffer.cpp
        vulkan/image.cpp
        vulkan/image_view.cpp
        vulkan/render_pass.cpp
        vulkan/rendering_context.cpp
        vulkan/rendering_pipeline.cpp
        vulkan/sampler.cpp
        vulkan/sampler.hpp
        vulkan/shader.cpp
        vulkan/utils.cpp
        vulkan/vertex_buffer_layout.cpp
)
set_source_files_properties(
        impl/stb_impl.cpp
        impl/tiny_gltf.cpp
        impl/vma_impl.cpp
        impl/volk_impl.cpp
        PROPERTIES
        COMPILE_FLAGS "-Wno-everything"
)
target_compile_definitions(playground PRIVATE
        RESOURCE_DIR=\"${CMAKE_CURRENT_LIST_DIR}/../res/\")
target_include_directories(playground PRIVATE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(playground
        ${CMAKE_DL_LIBS}
        fmt
        glfw
        glm
        imgui
        magic_enum
        nlohmann_json
        shaders
        spdlog
        spirv-reflect-static
        stb
        tinygltf
        tinyobjloader
        TracyClient
        volk_headers
        Vulkan-Headers
        VulkanMemoryAllocator
)
target_compile_options(playground PRIVATE -Wall -Wextra)

##ASAN
#target_compile_options(playground PRIVATE -fsanitize=address)
#target_link_options(playground PRIVATE -fsanitize=address)
##LSAN
#target_compile_options(playground PRIVATE -fsanitize=leak)
#target_link_options(playground PRIVATE -fsanitize=leak)
##MSAN
#target_compile_options(playground PRIVATE -fsanitize=memory)
#target_link_options(playground PRIVATE -fsanitize=memory)
#TSAN
#target_compile_options(playground PRIVATE -fsanitize=thread)
#target_link_options(playground PRIVATE -fsanitize=thread)
##UBSAN
#target_compile_options(playground PRIVATE -fsanitize=undefined)
#target_link_options(playground PRIVATE -fsanitize=undefined)

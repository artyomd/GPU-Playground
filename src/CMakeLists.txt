add_subdirectory(shaders)

add_executable(playground
        application/application.cpp
        application/application.hpp
        application/renderable.hpp
        geometry/color.hpp
        geometry/geometry_item.cpp
        geometry/geometry_item.hpp
        geometry/gltf_model.cpp
        geometry/gltf_model.hpp
        geometry/gltf_model_defaults.hpp
        geometry/plane_2d.cpp
        geometry/plane_2d.hpp
        geometry/point.hpp
        geometry/quad.cpp
        geometry/quad.hpp
        geometry/spiral_sphere.cpp
        geometry/spiral_sphere.hpp
        geometry/stacked_sphere.cpp
        geometry/stacked_sphere.hpp
        geometry/triangle.cpp
        geometry/triangle.hpp
        impl/stb_impl.cpp
        impl/tiny_gltf.cpp
        impl/vma_impl.cpp
        impl/volk_impl.cpp
        include/vulkan_glfw.hpp
        include/vulkan_include.hpp
        main.cpp
        renderables/imgui_wrapper.cpp
        renderables/imgui_wrapper.hpp
        renderables/menu/default_menu.cpp
        renderables/menu/default_menu.hpp
        renderables/menu/menu.hpp
        renderables/menu/menu_manager.cpp
        renderables/menu/menu_manager.hpp
        renderables/model/cube.cpp
        renderables/model/cube.hpp
        renderables/model/gltf_model.cpp
        renderables/model/gltf_model.hpp
        renderables/model/lighting.cpp
        renderables/model/lighting.hpp
        renderables/model/model.cpp
        renderables/model/model.hpp
        renderables/model/obj_model.cpp
        renderables/model/obj_model.hpp
        renderables/model/spiral_sphere.cpp
        renderables/model/spiral_sphere.hpp
        renderables/model/stacked_sphere.cpp
        renderables/model/stacked_sphere.hpp
        renderables/model/texture_2d.cpp
        renderables/model/texture_2d.hpp
        renderables/model/triangle.cpp
        renderables/model/triangle.hpp
        renderables/particles/particles.cpp
        renderables/particles/particles.cpp
        renderables/shader.cpp
        renderables/shader.hpp
        vulkan/buffer.cpp
        vulkan/buffer.hpp
        vulkan/compute_pipeline.cpp
        vulkan/compute_pipeline.hpp
        vulkan/framebuffer.cpp
        vulkan/framebuffer.hpp
        vulkan/image.cpp
        vulkan/image.hpp
        vulkan/image_view.cpp
        vulkan/image_view.hpp
        vulkan/render_pass.cpp
        vulkan/render_pass.hpp
        vulkan/rendering_context.cpp
        vulkan/rendering_context.hpp
        vulkan/rendering_pipeline.cpp
        vulkan/rendering_pipeline.hpp
        vulkan/sampler.cpp
        vulkan/sampler.hpp
        vulkan/semaphore_pool.cpp
        vulkan/semaphore_pool.hpp
        vulkan/shader.cpp
        vulkan/shader.hpp
        vulkan/utils.cpp
        vulkan/utils.hpp
        vulkan/vertex_buffer_layout.cpp
        vulkan/vertex_buffer_layout.hpp
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

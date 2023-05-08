set(VULKAN_SDK_VERSION sdk-1.3.243.0)

FetchContent_Declare(glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.3.8
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
        )
set(BUILD_SHARED_LIBS OFF)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 9.1.0
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
        )
FetchContent_MakeAvailable(fmt)

FetchContent_Declare(spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.11.0
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
        )
set(SPDLOG_FMT_EXTERNAL ON)
FetchContent_MakeAvailable(spdlog)

FetchContent_Declare(glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 0.9.9.8
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
        )
FetchContent_MakeAvailable(glm)

FetchContent_Declare(SPIRV-Reflect
        GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Reflect.git
        GIT_TAG ${VULKAN_SDK_VERSION}
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
        )
set(SPIRV_REFLECT_EXAMPLES OFF)
set(SPIRV_REFLECT_EXECUTABLE OFF)
set(SPIRV_REFLECT_STATIC_LIB ON)
FetchContent_MakeAvailable(SPIRV-Reflect)

FetchContent_Declare(volk
        GIT_REPOSITORY https://github.com/zeux/volk.git
        GIT_TAG ${VULKAN_SDK_VERSION}
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
        )
set(VOLK_PULL_IN_VULKAN OFF)
FetchContent_MakeAvailable(volk)

FetchContent_Declare(Vulkan-Headers
        GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Headers.git
        GIT_TAG ${VULKAN_SDK_VERSION}
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
        )
FetchContent_MakeAvailable(Vulkan-Headers)

FetchContent_Declare(VulkanMemoryAllocator
        GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git
        GIT_TAG v3.0.1
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        )
FetchContent_Populate(VulkanMemoryAllocator)
add_library(VulkanMemoryAllocator INTERFACE)
target_include_directories(VulkanMemoryAllocator INTERFACE
        ${vulkanmemoryallocator_SOURCE_DIR}/include
        )
target_compile_definitions(
        VulkanMemoryAllocator INTERFACE
        VMA_STATIC_VULKAN_FUNCTIONS=false
        VMA_DYNAMIC_VULKAN_FUNCTIONS=false
)

FetchContent_Declare(imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG v1.89.5
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        )
FetchContent_MakeAvailable(imgui)
### for some reason imgui does not provide a cmake project
add_library(imgui STATIC
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        )
target_compile_definitions(imgui PUBLIC
        -DIMGUI_IMPL_VULKAN_NO_PROTOTYPES
        -DIMGUI_DISABLE_DEMO_WINDOWS
        -DGLFW_INCLUDE_NONE)
target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR}
        PUBLIC ${imgui_SOURCE_DIR}/backends)
target_link_libraries(imgui PRIVATE glfw Vulkan-Headers)

FetchContent_Declare(json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.2
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
        )
FetchContent_MakeAvailable(json)

FetchContent_Declare(stb
        GIT_REPOSITORY https://github.com/nothings/stb.git
        GIT_TAG 5736b15f7ea0ffb08dd38af21067c314d6a3aae9
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
        )
FetchContent_MakeAvailable(stb)
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE ${stb_SOURCE_DIR})

FetchContent_Declare(tinygltf
        GIT_REPOSITORY https://github.com/syoyo/tinygltf.git
        GIT_TAG v2.8.9
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
        )
set(TINYGLTF_BUILD_LOADER_EXAMPLE OFF)
set(TINYGLTF_INSTALL OFF)
set(TINYGLTF_HEADER_ONLY ON)
FetchContent_MakeAvailable(tinygltf)

FetchContent_Declare(tinyobj
        GIT_REPOSITORY https://github.com/tinyobjloader/tinyobjloader.git
        GIT_TAG v2.0.0rc10
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
        )
FetchContent_MakeAvailable(tinyobj)

FetchContent_Declare(tracy
        GIT_REPOSITORY https://github.com/wolfpld/tracy.git
        GIT_TAG v0.9.1
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
        )
set(TRACY_ENABLE OFF)
set(TRACY_STATIC ON)
set(TRACY_CALLSTACK ON)
set(TRACY_ONLY_LOCALHOST ON)
set(TRACY_NO_EXIT ON)
FetchContent_MakeAvailable(tracy)
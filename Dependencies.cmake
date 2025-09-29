set(VULKAN_SDK_VERSION vulkan-sdk-1.4.321.0)

FetchContent_Declare(glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
)
set(BUILD_SHARED_LIBS OFF)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(magic_enum
        GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
        GIT_TAG v0.9.7
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(magic_enum)

FetchContent_Declare(fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 12.0.0
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
)
set(FMT_OS OFF)
FetchContent_MakeAvailable(fmt)

FetchContent_Declare(spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.15.3
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
)
set(SPDLOG_FMT_EXTERNAL ON)
FetchContent_MakeAvailable(spdlog)

FetchContent_Declare(glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 1.0.1
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
)
set(GLM_ENABLE_CXX_20, ON)
FetchContent_MakeAvailable(glm)

FetchContent_Declare(SPIRV-Reflect
        GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Reflect.git
        GIT_TAG ${VULKAN_SDK_VERSION}
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
)
set(SPIRV_REFLECT_EXECUTABLE OFF)
set(SPIRV_REFLECT_STATIC_LIB ON)
set(SPIRV_REFLECT_INSTALL OFF)
FetchContent_MakeAvailable(SPIRV-Reflect)

FetchContent_Declare(volk
        GIT_REPOSITORY https://github.com/zeux/volk.git
        GIT_TAG ${VULKAN_SDK_VERSION}
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
)
set(VOLK_PULL_IN_VULKAN OFF)
set(VOLK_HEADERS_ONLY ON)
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
        GIT_TAG v3.3.0
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(VulkanMemoryAllocator)

FetchContent_Declare(imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG v1.92.3
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
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
        -DGLFW_INCLUDE_NONE)
target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR}
        PUBLIC ${imgui_SOURCE_DIR}/backends)
target_link_libraries(imgui PRIVATE glfw Vulkan-Headers)

FetchContent_Declare(imgui-fb
        GIT_REPOSITORY https://github.com/AirGuanZ/imgui-filebrowser.git
        GIT_TAG 47a1884524d6a3df5d99f70ecc82e1ed0ccd26cf
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(imgui-fb)
add_library(imgui-fb INTERFACE)
target_include_directories(imgui-fb INTERFACE ${imgui-fb_SOURCE_DIR})

FetchContent_Declare(json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.12.0
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(json)

FetchContent_Declare(stb
        GIT_REPOSITORY https://github.com/nothings/stb.git
        GIT_TAG fede005abaf93d9d7f3a679d1999b2db341b360f
        GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(stb)
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE ${stb_SOURCE_DIR})

FetchContent_Declare(tinygltf
        GIT_REPOSITORY https://github.com/syoyo/tinygltf.git
        GIT_TAG v2.9.6
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
)
set(TINYGLTF_BUILD_LOADER_EXAMPLE OFF)
set(TINYGLTF_INSTALL OFF)
set(TINYGLTF_INSTALL_VENDOR OFF)
FetchContent_MakeAvailable(tinygltf)

FetchContent_Declare(tinyobj
        GIT_REPOSITORY https://github.com/tinyobjloader/tinyobjloader.git
        GIT_TAG v2.0.0rc13
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(tinyobj)

FetchContent_Declare(tracy
        GIT_REPOSITORY https://github.com/wolfpld/tracy.git
        GIT_TAG v0.12.2
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
)
set(TRACY_ENABLE OFF)
set(TRACY_STATIC ON)
set(TRACY_CALLSTACK ON)
set(TRACY_ONLY_LOCALHOST ON)
set(TRACY_NO_EXIT ON)
FetchContent_MakeAvailable(tracy)

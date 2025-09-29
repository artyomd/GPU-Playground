set(VULKAN_SDK_VERSION vulkan-sdk-1.4.321.0)

FetchContent_Declare(SPIRV-Headers
        GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Headers.git
        GIT_TAG ${VULKAN_SDK_VERSION}
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(SPIRV-Headers)

FetchContent_Declare(SPIRV-Tools
        GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Tools.git
        GIT_TAG ${VULKAN_SDK_VERSION}
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
)
set(SKIP_SPIRV_TOOLS_INSTALL ON)
set(SPIRV_SKIP_EXECUTABLES ON)
set(SPIRV_SKIP_TESTS ON)
set(SPIRV_TOOLS_BUILD_STATIC ON)
FetchContent_MakeAvailable(SPIRV-Tools)

FetchContent_Declare(glslang
        GIT_REPOSITORY https://github.com/KhronosGroup/glslang.git
        GIT_TAG ${VULKAN_SDK_VERSION}
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
)
set(BUILD_SHARED_LIBS OFF)
set(BUILD_EXTERNAL OFF)
set(ENABLE_SPVREMAPPER OFF)
set(ENABLE_GLSLANG_BINARIES OFF)
set(ENABLE_GLSLANG_JS OFF)
set(ALLOW_EXTERNAL_SPIRV_TOOLS ON)
FetchContent_MakeAvailable(glslang)

FetchContent_Declare(shaderc
        GIT_REPOSITORY https://github.com/google/shaderc.git
        GIT_TAG v2025.4
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
)
set(SHADERC_ENABLE_WGSL_OUTPUT OFF)
set(SHADERC_SKIP_INSTALL ON)
set(SHADERC_SKIP_TESTS ON)
set(SHADERC_SKIP_EXAMPLES ON)
set(SHADERC_SKIP_COPYRIGHT_CHECK ON)
set(SHADERC_ENABLE_WERROR_COMPILE ON)
FetchContent_MakeAvailable(shaderc)

#add spirv library
#LIBRARY_NAME - string, name of output library target
#DEBUG -  boolean, enable/disable, generate debuggable spir-v shaders
#WERROR - boolean, enable/disable, treat all warnings as errors.
#TARGET_ENV - string, target client environment
#TARGET_SPV - string, target spv version
#INCLUDE_PATH - string, include path for shaders
#INPUT_FILES - list, absolute path to source files
FUNCTION(add_spirv_library)
    cmake_parse_arguments(PARAM "" "LIBRARY_NAME;DEBUG;WERROR;TARGET_ENV;TARGET_SPV;INCLUDE_PATH" "INPUT_FILES" ${ARGN})

    set(EXTRA_FLAGS)
    if (${PARAM_DEBUG})
        set(EXTRA_FLAGS "${EXTRA_FLAGS};-O0;-g")
    else ()
        set(EXTRA_FLAGS "${EXTRA_FLAGS};-O")
    endif ()

    if (${PARAM_WERROR})
        set(EXTRA_FLAGS "${EXTRA_FLAGS};-Werror")
    endif ()

    if (NOT ${TARGET_SPV} STREQUAL "")
        set(EXTRA_FLAGS "${EXTRA_FLAGS};--target-spv=${TARGET_SPV}")
    endif ()

    if (NOT ${TARGET_ENV} STREQUAL "")
        set(EXTRA_FLAGS "${EXTRA_FLAGS};--target-env=${TARGET_ENV}")
    endif ()

    if (NOT ${INCLUDE_PATH} STREQUAL "")
        set(EXTRA_FLAGS "${EXTRA_FLAGS};-I;${TARGET_SPV}")
    endif ()

    set(SPIR_V_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/spv")

    add_custom_target(CREATE_SPIRV_BUILD_DIR
            COMMAND ${CMAKE_COMMAND} -E make_directory ${SPIR_V_DIRECTORY})

    FOREACH (FILE ${PARAM_INPUT_FILES})
        cmake_path(GET FILE STEM LAST_ONLY FILE_NAME)
        set(OUTPUT_FILE "${SPIR_V_DIRECTORY}/${FILE_NAME}.spv")
        add_custom_command(OUTPUT ${OUTPUT_FILE}
                COMMAND glslc_exe ${EXTRA_FLAGS} -mfmt=num -o ${OUTPUT_FILE} ${FILE}
                COMMENT "Building shader object ${OUTPUT_FILE}"
                DEPENDS glslc_exe CREATE_SPIRV_BUILD_DIR ${FILE}
        )

        list(APPEND OUTPUT_SPV_FILES ${OUTPUT_FILE})
    ENDFOREACH (FILE)

    add_library(${PARAM_LIBRARY_NAME} INTERFACE ${OUTPUT_SPV_FILES})
    target_include_directories(${PARAM_LIBRARY_NAME} INTERFACE
            ${SPIR_V_DIRECTORY})

ENDFUNCTION(add_spirv_library)

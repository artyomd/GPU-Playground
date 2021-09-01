cmake_minimum_required(VERSION 3.20)

project(playground)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ThreadSanitizer
#set(CMAKE_C_FLAGS "-fsanitize=thread -g -O1")
#set(CMAKE_CXX_FLAGS "-fsanitize=thread -g -O1")
# AddressSanitize
#set(CMAKE_C_FLAGS "-fsanitize=address")
#set(CMAKE_CXX_FLAGS "-fsanitize=address")
# LeakSanitizer
#set(CMAKE_C_FLAGS "-fsanitize=leak -fno-omit-frame-pointer -g -O1")
#set(CMAKE_CXX_FLAGS "-fsanitize=leak -fno-omit-frame-pointer -g -O1")
# MemorySanitizer
#set(CMAKE_C_FLAGS "-fsanitize=memory -fno-optimize-sibling-calls -fsanitize-memory-track-origins=2 -fno-omit-frame-pointer -g -O2")
#set(CMAKE_CXX_FLAGS"-fsanitize=memory -fno-optimize-sibling-calls -fsanitize-memory-track-origins=2 -fno-omit-frame-pointer -g -O2")
# UndefinedBehaviour
#set(CMAKE_C_FLAGS"-fsanitize=undefined")
#set(CMAKE_CXX_FLAGS "-fsanitize=undefined")

find_package(glfw3 REQUIRED)

IF (NOT APPLE)
    OPTION(USE_OPENGL "Build with opengl support" ON)
ENDIF ()

find_package(Vulkan)
IF (NOT Vulkan_FOUND)
    message(FATAL_ERROR "Could not find Vulkan library!")
ELSE ()
    include_directories(${Vulkan_INCLUDE_DIRS})
ENDIF ()

file(GLOB_RECURSE SOURCE_FILES src/*.cpp src/*.c src/*.hpp src/*.h)

file(GLOB_RECURSE EXTERNAL_SOURCE_FILES external/src/*.cpp)
#spir-v reflect
list(APPEND EXTERNAL_SOURCE_FILES external/SPIRV-Reflect/spirv_reflect.c)

IF (USE_OPENGL)
    MESSAGE("gl enabled")
    SET(OPEN_GL GL)
    find_package(GLEW REQUIRED)
    add_definitions(${GLEW_DEFINITIONS})
    add_compile_definitions(GLEW_STATIC)
ELSE (USE_OPENGL)
    MESSAGE("gl disabled")
    list(FILTER SOURCE_FILES EXCLUDE REGEX "^.*opengl.*$")
    list(FILTER EXTERNAL_SOURCE_FILES EXCLUDE REGEX "^.*opengl.*$")
ENDIF (USE_OPENGL)

add_definitions(-DIMGUI_DISABLE_DEMO_WINDOWS)

add_subdirectory(external/snowhouse)

FUNCTION(CONVERT_SPV result)
    SET(spvs "")
    FOREACH (glsl ${ARGN})
        get_filename_component(name ${glsl} NAME_WE)
        LIST(APPEND spvs "${CMAKE_CURRENT_SOURCE_DIR}/res/shader/spv/${name}.spv")
        LIST(APPEND spvs "${CMAKE_CURRENT_SOURCE_DIR}/res/shader/spv/.hex/${name}.spv")
    ENDFOREACH (glsl)
    SET(${result} "${spvs}" PARENT_SCOPE)
ENDFUNCTION(CONVERT_SPV)

file(GLOB_RECURSE GLSL_FILES "${CMAKE_CURRENT_SOURCE_DIR}/res/shader/glsl460/*.glsl")

CONVERT_SPV(SPV_FILES ${GLSL_FILES})

add_custom_command(
        OUTPUT ${SPV_FILES}
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/res/shader/generate-spv.sh
        DEPENDS ${GLSL_FILES}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/res/shader/
)

set_source_files_properties(
        ${SOURCE_FILES}
        PROPERTIES
        COMPILE_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic -Wconversion -Wfloat-equal -Wold-style-cast"
)

add_executable(playground ${SOURCE_FILES} ${SPV_FILES} ${EXTERNAL_SOURCE_FILES})

target_include_directories(playground PRIVATE "${PROJECT_SOURCE_DIR}")
target_include_directories(playground SYSTEM PRIVATE ${GLEW_INCLUDE_DIRS})
target_include_directories(playground SYSTEM PRIVATE external/glm)
target_include_directories(playground SYSTEM PRIVATE external/stb)
target_include_directories(playground SYSTEM PRIVATE external/tinyobjloader)
target_include_directories(playground SYSTEM PRIVATE external/src)
target_include_directories(playground SYSTEM PRIVATE external/src/imgui)
target_include_directories(playground SYSTEM PRIVATE external/json/single_include/)
target_include_directories(playground SYSTEM PRIVATE external)

target_include_directories(playground SYSTEM PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/res/shader/spv/.hex")

target_link_libraries(playground glfw ${OPEN_GL} ${GLEW_LIBRARIES} ${Vulkan_LIBRARY} snowhouse ${CMAKE_DL_LIBS})

cmake_minimum_required(VERSION 3.16.5)

project(playground)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ThreadSanitizer
#set(CMAKE_C_FLAGS
#        "-fsanitize=thread -g -O1"
#        CACHE STRING "Flags used by the C compiler during ThreadSanitizer builds."
#        FORCE)
#set(CMAKE_CXX_FLAGS
#        "-fsanitize=thread -g -O1"
#        CACHE STRING "Flags used by the C++ compiler during ThreadSanitizer builds."
#        FORCE)
#
# AddressSanitize
#set(CMAKE_C_FLAGS
#        "-fsanitize=address"
#        CACHE STRING "Flags used by the C compiler during AddressSanitizer builds."
#        FORCE)
#set(CMAKE_CXX_FLAGS
#        "-fsanitize=address"
#        CACHE STRING "Flags used by the C++ compiler during AddressSanitizer builds."
#        FORCE)

# LeakSanitizer
#set(CMAKE_C_FLAGS
#        "-fsanitize=leak -fno-omit-frame-pointer -g -O1"
#        CACHE STRING "Flags used by the C compiler during LeakSanitizer builds."
#        FORCE)
#set(CMAKE_CXX_FLAGS
#        "-fsanitize=leak -fno-omit-frame-pointer -g -O1"
#        CACHE STRING "Flags used by the C++ compiler during LeakSanitizer builds."
#        FORCE)
#
# MemorySanitizer
#set(CMAKE_C_FLAGS
#        "-fsanitize=memory -fno-optimize-sibling-calls -fsanitize-memory-track-origins=2 -fno-omit-frame-pointer -g -O2"
#        CACHE STRING "Flags used by the C compiler during MemorySanitizer builds."
#        FORCE)
#set(CMAKE_CXX_FLAGS
#        "-fsanitize=memory -fno-optimize-sibling-calls -fsanitize-memory-track-origins=2 -fno-omit-frame-pointer -g -O2"
#        CACHE STRING "Flags used by the C++ compiler during MemorySanitizer builds."
#        FORCE)
#
# UndefinedBehaviour
#set(CMAKE_C_FLAGS
#        "-fsanitize=undefined"
#        CACHE STRING "Flags used by the C compiler during UndefinedBehaviourSanitizer builds."
#        FORCE)
#set(CMAKE_CXX_FLAGS
#        "-fsanitize=undefined"
#        CACHE STRING "Flags used by the C++ compiler during UndefinedBehaviourSanitizer builds."
#        FORCE)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

OPTION(USE_D2D_WSI "Build the project using Direct to Display swapchain" OFF)
OPTION(USE_WAYLAND_WSI "Build the project using Wayland swapchain" ON)

find_package(glfw3 REQUIRED)

find_package(GLEW REQUIRED)
add_definitions(${GLEW_DEFINITIONS})
add_compile_definitions(GLEW_STATIC)
IF (APPLE)
    FIND_LIBRARY(OpenGL_LIBRARY OpenGL)
    SET(OPEN_GL ${OpenGL_LIBRARY})
    OPTION(USE_D2D_WSI "Build the project using Direct to Display swapchain" ON)
    OPTION(USE_WAYLAND_WSI "Build the project using Wayland swapchain" OFF)
ELSE ()
    SET(OPEN_GL GL)
ENDIF (APPLE)

find_package(Vulkan)
IF (NOT Vulkan_FOUND)
    message(FATAL_ERROR "Could not find Vulkan library!")
ELSE ()
    include_directories(${Vulkan_INCLUDE_DIRS})
ENDIF ()

IF (USE_D2D_WSI)
    MESSAGE("Using direct to display extension...")
    add_definitions(-D_DIRECT2DISPLAY)
ELSEIF (USE_WAYLAND_WSI)
    find_program(PKG_CONFIG pkg-config)
    if (NOT PKG_CONFIG)
        message(FATAL_ERROR "pkg-config binary not found")
    endif ()
    find_package(Wayland REQUIRED)
    if (NOT WAYLAND_FOUND)
        message(FATAL_ERROR "Wayland development package not found")
    endif ()
    pkg_check_modules(WAYLAND_PROTOCOLS REQUIRED wayland-protocols)
    if (NOT WAYLAND_PROTOCOLS_FOUND)
        message(FATAL_ERROR "Wayland protocols package not found")
    endif ()
    find_program(WAYLAND_SCANNER wayland-scanner)
    if (NOT WAYLAND_SCANNER)
        message(FATAL_ERROR "wayland-scanner binary not found")
    endif ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_WAYLAND_KHR")
    include_directories(${WAYLAND_INCLUDE_DIR})
    execute_process(COMMAND ${PKG_CONFIG} --variable=pkgdatadir wayland-protocols OUTPUT_VARIABLE protocol_dir OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND ${WAYLAND_SCANNER} client-header ${protocol_dir}/stable/xdg-shell/xdg-shell.xml ${CMAKE_BINARY_DIR}/xdg-shell-client-protocol.h
            COMMAND ${WAYLAND_SCANNER} private-code ${protocol_dir}/stable/xdg-shell/xdg-shell.xml ${CMAKE_BINARY_DIR}/xdg-shell-protocol.c)
    include_directories(${CMAKE_BINARY_DIR})
ELSE (USE_D2D_WSI)
    find_package(XCB REQUIRED)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_XCB_KHR")
ENDIF (USE_D2D_WSI)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp src/*.hpp src/*.h)

include_directories("${PROJECT_SOURCE_DIR}/src")
include_directories("${PROJECT_SOURCE_DIR}/src/api")
include_directories("${PROJECT_SOURCE_DIR}/src/application")
include_directories("${PROJECT_SOURCE_DIR}/src/geometry")
include_directories("${PROJECT_SOURCE_DIR}/src/vendor")
include_directories("${PROJECT_SOURCE_DIR}/src/test")

add_executable(playground ${SOURCE_FILES})
target_link_libraries(playground glfw ${OPEN_GL} ${GLEW_LIBRARIES} ${Vulkan_LIBRARY} ${XCB_LIBRARIES} ${WAYLAND_CLIENT_LIBRARIES})
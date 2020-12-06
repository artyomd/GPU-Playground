cmake_minimum_required(VERSION 3.17.3)

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
#
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

IF (UNIX AND NOT APPLE)
    set(LINUX TRUE)
ENDIF ()

find_package(glfw3 REQUIRED)

IF (APPLE)
    MESSAGE("Apple")
ELSE (APPLE)
    MESSAGE("not Apple")
    OPTION(USE_OPENGL "Build with opengl support" ON)
ENDIF (APPLE)

find_package(Vulkan)
IF (NOT Vulkan_FOUND)
    message(FATAL_ERROR "Could not find Vulkan library!")
ELSE ()
    include_directories(${Vulkan_INCLUDE_DIRS})
ENDIF ()

file(GLOB_RECURSE SOURCE_FILES src/*.cpp src/*.c src/*.hpp src/*.h)

IF (USE_OPENGL)
    MESSAGE("gl enabled")
    SET(OPEN_GL GL)
    find_package(GLEW REQUIRED)
    add_definitions(${GLEW_DEFINITIONS})
    add_compile_definitions(GLEW_STATIC)
ELSE (USE_OPENGL)
    MESSAGE("gl disabled")
    list(FILTER SOURCE_FILES EXCLUDE REGEX "opengl")
ENDIF (USE_OPENGL)

include_directories("${PROJECT_SOURCE_DIR}")
include_directories(SYSTEM "${PROJECT_SOURCE_DIR}/src/vendor")
include_directories(SYSTEM ${GLEW_INCLUDE_DIRS})

add_executable(playground ${SOURCE_FILES})
target_link_libraries(playground glfw ${OPEN_GL} ${GLEW_LIBRARIES} ${Vulkan_LIBRARY} ${DIRECTFB_LIBRARIES} ${XCB_LIBRARIES} ${WAYLAND_CLIENT_LIBRARIES})

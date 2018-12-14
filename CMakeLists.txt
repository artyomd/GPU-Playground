cmake_minimum_required(VERSION 3.13)
project(playground)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)

add_definitions(${GLEW_DEFINITIONS})

add_compile_definitions(GLEW_STATIC)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ../out)

file(GLOB_RECURSE SOURCE_FILES_C src/*.cpp src/*.hpp src/*.h)

add_executable(playground ${SOURCE_FILES_C})

include_directories("${PROJECT_SOURCE_DIR}/src")
include_directories("${PROJECT_SOURCE_DIR}/src/geometry")
include_directories("${PROJECT_SOURCE_DIR}/src/vendor")
include_directories("${PROJECT_SOURCE_DIR}/src/test")

IF (APPLE)
    FIND_LIBRARY(OpenGL_LIBRARY OpenGL)
    MARK_AS_ADVANCED(COCOA_LIBRARY
            GLUT_LIBRARY
            OpenGL_LIBRARY)
    SET(OPEN_GL ${OpenGL_LIBRARY})
ELSE()
    SET(OPEN_GL GL)
ENDIF (APPLE)

target_link_libraries (playground glfw ${OPEN_GL} ${OPENGL_gl_LIBRARY} ${GLEW_LIBRARIES})
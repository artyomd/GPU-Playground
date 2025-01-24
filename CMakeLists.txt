cmake_minimum_required(VERSION 3.30)
include(FetchContent)

project(playground CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

include(ShaderCompiler.cmake)
include(Dependencies.cmake)

add_subdirectory(src)

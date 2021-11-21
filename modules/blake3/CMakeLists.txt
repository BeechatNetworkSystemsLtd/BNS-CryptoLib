cmake_minimum_required(VERSION 3.10)

project(blake3jni C ASM)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

enable_testing()

add_custom_target(blake3_multiarch_libs)
add_custom_target(blake3_android_libs)
add_custom_target(blake3_x86_libs)

add_subdirectory(src)
add_subdirectory(test)


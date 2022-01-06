cmake_minimum_required(VERSION 3.10)

project(blake3jni_android C ASM)

include(../compile_options.cmake)
include(../source.cmake)

set(CMAKE_SYSTEM_NAME Android)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_SYSROOT ${ndk}/toolchains/llvm/prebuilt/linux-x86_64/sysroot)
set(CMAKE_C_COMPILER ${ndk}/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android29-clang)
set(CMAKE_CXX_COMPILER ${ndk}/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android29-clang++)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

add_library(blake3_jni SHARED ${BLAKE3_SRCS} ${BLAKE3_ARM_SRCS})

target_compile_options(blake3_jni PUBLIC -march=armv8-a -Ofast -Wno-missing-braces
    -Wno-missing-field-initializers -Wno-missing-prototypes -Wno-redundant-decls)
target_compile_definitions(blake3_jni PUBLIC BLAKE3_USE_NEON=1)


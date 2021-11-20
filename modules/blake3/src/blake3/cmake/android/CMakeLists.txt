cmake_minimum_required(VERSION 3.10)

project(blake3jni_android C ASM)

include(../compile_options.cmake)
include(../source.cmake)

set(CMAKE_SYSTEM_NAME Android)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_SYSROOT ${ndk}/platforms/android-21/arch-arm64)
set(CMAKE_C_COMPILER ${ndk}/toolchains/aarch64-linux-android-4.9/prebuilt/linux-x86_64/bin/aarch64-linux-android-gcc)
set(CMAKE_CXX_COMPILER ${ndk}/toolchains/aarch64-linux-android-4.9/prebuilt/linux-x86_64/bin/aarch64-linux-android-g++)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

add_library(blake3_android_jni SHARED ${BLAKE3_SRCS})

target_compile_options(blake3_android_jni PUBLIC -march=armv8-a -fPIC -Wno-missing-braces
    -Wno-missing-field-initializers)
target_compile_definitions(blake3_android_jni PUBLIC BLAKE3_USE_NEON=1)

add_custom_command(
  TARGET blake3_android_jni POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory ${release_dir}/android
)
if (MSVC)
  add_custom_command(
    TARGET blake3_android_jni POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "blake3_android_jni.dll" "${release_dir}/android"
    COMMENT "Making Release: blake3_android_jni.dll"
  )
elseif(MSYS OR MINGW)
  add_custom_command(
    TARGET blake3_android_jni POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "libblake3_android_jni.dll" "${release_dir}/android"
    COMMENT "Making Release: libblake3_android_jni.dll"
  )
elseif(UNIX)
  add_custom_command(
    TARGET blake3_android_jni POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "libblake3_android_jni.so" "${release_dir}/android"
    COMMENT "Making Release: libblake3_android_jni.so"
  )
endif()


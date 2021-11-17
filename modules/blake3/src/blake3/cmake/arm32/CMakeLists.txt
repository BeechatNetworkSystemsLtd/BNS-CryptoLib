cmake_minimum_required(VERSION 3.10)

project(blake3jni_arm32 C ASM)

include(../compile_options.cmake)
include(../source.cmake)

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_SYSROOT ${arm32_toolchain}/arm-linux-gnueabihf/libc)
set(CMAKE_C_COMPILER ${arm32_toolchain}/bin/arm-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER ${arm32_toolchain}/bin/arm-linux-gnueabihf-g++)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)


add_library(blake3_arm32_jni SHARED ${BLAKE3_SRCS})

add_custom_command(
  TARGET blake3_arm32_jni POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory ${release_dir}/arm32
)
if (MSVC)
  add_custom_command(
    TARGET blake3_arm32_jni POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "blake3_arm32_jni.dll" "${release_dir}/arm32"
    COMMENT "Making Release: blake3_arm32_jni.dll"
  )
elseif(MSYS OR MINGW)
  add_custom_command(
    TARGET blake3_arm32_jni POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "libblake3_arm32_jni.dll" "${release_dir}/arm32"
    COMMENT "Making Release: libblake3_arm32_jni.dll"
  )
elseif(UNIX)
  add_custom_command(
    TARGET blake3_arm32_jni POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "libblake3_arm32_jni.so" "${release_dir}/arm32"
    COMMENT "Making Release: libblake3_arm32_jni.so"
  )
endif()



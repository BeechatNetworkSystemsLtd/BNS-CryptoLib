cmake_minimum_required(VERSION 3.10)

project(blake3jni_x86 C ASM)

include(../compile_options.cmake)
include(../source.cmake)

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR x86)

if(MSVC)
    add_library(blake3_jni SHARED ${BLAKE3_SRCS} ${BLAKE3_X86_MSVC_SRCS})
elseif(MSYS OR MINGW)
    add_library(blake3_jni SHARED ${BLAKE3_SRCS} ${BLAKE3_X86_MINGW_SRCS})
elseif(UNIX)
    add_library(blake3_jni SHARED ${BLAKE3_SRCS} ${BLAKE3_X86_UNIX_SRCS})
endif()

add_custom_command(
  TARGET blake3_jni POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory ${release_dir}/x86
)
if (MSVC)
  add_custom_command(
    TARGET blake3_jni POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "blake3_jni.dll" "${release_dir}/x86"
    COMMENT "Making Release: blake3_jni.dll (x86)"
  )
elseif(MSYS OR MINGW)
  add_custom_command(
    TARGET blake3_jni POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "libblake3_jni.dll" "${release_dir}/x86"
    COMMENT "Making Release: libblake3_jni.dll (x86)"
  )
elseif(UNIX)
  add_custom_command(
    TARGET blake3_jni POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "libblake3_jni.so" "${release_dir}/x86"
    COMMENT "Making Release: libblake3_jni.so (x86)"
  )
endif()


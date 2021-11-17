set(BLAKE3_SRCS
    blake3.c
    network_beechat_Blake3.c
    blake3_dispatch.c
    blake3_portable.c
)
set(BLAKE3_X86_UNIX_SRCS
    blake3_sse2_x86-64_unix.S
    blake3_sse41_x86-64_unix.S
    blake3_avx2_x86-64_unix.S
    blake3_avx512_x86-64_unix.S
)
set(BLAKE3_X86_MSVC_SRCS
    blake3_sse2_x86-64_windows_msvc.asm
    blake3_sse41_x86-64_windows_msvc.asm
    blake3_avx2_x86-64_windows_msvc.asm
    blake3_avx512_x86-64_windows_msvc.asm
)
set(BLAKE3_X86_MINGW_SRCS
    blake3_sse2_x86-64_windows_gnu.S
    blake3_sse41_x86-64_windows_gnu.S
    blake3_avx2_x86-64_windows_gnu.S
    blake3_avx512_x86-64_windows_gnu.S
)

find_package(Java)
find_package(Java COMPONENTS Development)

add_custom_target(blake3_jni)

add_custom_command(
  TARGET blake3_jni POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/Release
)
add_custom_command(
  TARGET blake3_jni POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/Release/headers
)
add_custom_command(
    TARGET blake3_jni POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/src/java"
"${CMAKE_BINARY_DIR}/Release/headers"
    COMMENT "Making Release: Java Classes"
)

if(MSVC)
  add_compile_options(/I${_JAVA_HOME}\\include /I${_JAVA_HOME}\\include\\win32)
  add_compile_options(/nologo /O2 /W4 /wd4146 /wd4244)
else()
  if(MSYS OR MINGW)
    add_compile_options(-I${_JAVA_HOME}\\include -I${_JAVA_HOME}\\include\\win32)
  else()
    add_compile_options(-I${_JAVA_HOME}/include -I${_JAVA_HOME}/include/linux)
  endif()
  add_compile_options(-Wall -fPIC -Wextra -Wpedantic -Werror -Wno-array-bounds -Wno-unused-function)
  add_compile_options(-Wmissing-prototypes -Wredundant-decls -Wshadow -Wpointer-arith)
  add_compile_options(-O3 -fomit-frame-pointer)
endif()

include(ExternalProject)

ExternalProject_Add(blake3jni_arm32
  PREFIX blake3jni_arm32
  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/arm32"
  CMAKE_CACHE_ARGS -Darm32_toolchain:PATH=${CMAKE_SOURCE_DIR}/../../toolchain/arm-linux-gnueabihf
  INSTALL_COMMAND ""
)

add_dependencies(blake3jni_arm32 blake3_jni)
#include(x86.cmake)
#include(arm64.cmake)
#include(arm32.cmake)


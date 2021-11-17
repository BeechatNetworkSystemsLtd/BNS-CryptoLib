cmake_minimum_required(VERSION 3.10)

project(blake3jni_arm32 C ASM)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)


set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_SYSROOT ${arm32_toolchain}/arm-linux-gnueabihf/libc)
set(CMAKE_C_COMPILER ${arm32_toolchain}/bin/arm-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER ${arm32_toolchain}/bin/arm-linux-gnueabihf-g++)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(BLAKE3_SRCS
    ../blake3.c
    ../network_beechat_Blake3.c
    ../blake3_dispatch.c
    ../blake3_portable.c
)

find_package(Java)
find_package(Java COMPONENTS Development)

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


add_library(blake3_arm32_jni SHARED ${BLAKE3_SRCS})

add_custom_command(
  TARGET blake3_arm32_jni POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/Release/arm32
)
if (MSVC)
  add_custom_command(
    TARGET blake3_arm32_jni POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "blake3_arm32_jni.dll" "${CMAKE_BINARY_DIR}/Release/arm32"
    COMMENT "Making Release: blake3_arm32_jni.dll"
  )
elseif(MSYS OR MINGW)
  add_custom_command(
    TARGET blake3_arm32_jni POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "libblake3_arm32_jni.dll" "${CMAKE_BINARY_DIR}/Release/arm32"
    COMMENT "Making Release: libblake3_arm32_jni.dll"
  )
elseif(UNIX)
  add_custom_command(
    TARGET blake3_arm32_jni POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "libblake3_arm32_jni.so" "${CMAKE_BINARY_DIR}/Release/arm32"
    COMMENT "Making Release: libblake3_arm32_jni.so"
  )
endif()



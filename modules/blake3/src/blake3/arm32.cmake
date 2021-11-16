set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_SYSROOT ${CMAKE_SOURCE_DIR}/../../toolchain/arm-linux-gnueabihf/arm-linux-gnueabihf/libc)

set(tools ${CMAKE_SOURCE_DIR}/../../toolchain/arm-linux-gnueabihf)
set(CMAKE_C_COMPILER ${tools}/bin/arm-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER ${tools}/bin/arm-linux-gnueabihf-g++)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)


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

add_dependencies(blake3_arm32_jni blake3_jni)


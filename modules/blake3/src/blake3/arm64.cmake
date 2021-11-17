project(blake3jni_arm64 C ASM)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

SET(CMAKE_FIND_ROOT_PATH ../../../../toolchain/aarch64-linux-gnu)

add_library(blake3_arm64_jni SHARED ${BLAKE3_SRCS})

target_compile_definitions(blake3_arm64_jni PUBLIC BLAKE3_USE_NEON=1)

add_custom_command(
  TARGET blake3_arm64_jni POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/Release/arm64
)
if (MSVC)
  add_custom_command(
    TARGET blake3_arm64_jni POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "blake3_arm64_jni.dll" "${CMAKE_BINARY_DIR}/Release/arm64"
    COMMENT "Making Release: blake3_arm32_jni.dll"
  )
elseif(MSYS OR MINGW)
  add_custom_command(
    TARGET blake3_arm64_jni POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "libblake3_arm64_jni.dll" "${CMAKE_BINARY_DIR}/Release/arm64"
    COMMENT "Making Release: libblake3_arm64_jni.dll"
  )
elseif(UNIX)
  add_custom_command(
    TARGET blake3_arm64_jni POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "libblake3_arm64_jni.so" "${CMAKE_BINARY_DIR}/Release/arm64"
    COMMENT "Making Release: libblake3_arm64_jni.so"
  )
endif()

add_dependencies(blake3_arm64_jni blake3_jni)


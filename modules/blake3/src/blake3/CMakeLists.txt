set(release_directory ${CMAKE_BINARY_DIR}/Release)

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

include(ExternalProject)

ExternalProject_Add(blake3jni_arm32
  PREFIX blake3jni_arm32
  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmake/arm32"
  CMAKE_CACHE_ARGS -Darm32_toolchain:PATH=${CMAKE_SOURCE_DIR}/../../toolchain/arm-linux-gnueabihf
                   -Drelease_dir:PATH=${release_directory}
  INSTALL_COMMAND ""
)

ExternalProject_Add(blake3jni_arm64
  PREFIX blake3jni_arm64
  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmake/arm64"
  CMAKE_CACHE_ARGS -Darm64_toolchain:PATH=${CMAKE_SOURCE_DIR}/../../toolchain/aarch64-linux-gnu
                   -Drelease_dir:PATH=${release_directory}
  INSTALL_COMMAND ""
)

ExternalProject_Add(blake3jni_x86
  PREFIX blake3jni_x86
  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmake/x86"
  CMAKE_CACHE_ARGS -Drelease_dir:PATH=${release_directory}
  INSTALL_COMMAND ""
)

ExternalProject_Add(blake3jni_android
  PREFIX blake3jni_android
  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmake/android"
  CMAKE_CACHE_ARGS -Drelease_dir:PATH=${release_directory}
                   -Dndk:PATH=${CMAKE_SOURCE_DIR}/../../toolchain/android-ndk-r13b
  INSTALL_COMMAND ""
)

add_dependencies(blake3jni_arm32 blake3_jni)
add_dependencies(blake3jni_arm64 blake3_jni)
add_dependencies(blake3jni_x86 blake3_jni)
add_dependencies(blake3jni_android blake3_jni)


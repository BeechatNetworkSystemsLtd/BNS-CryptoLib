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
  BUILD_ALWAYS 1
  INSTALL_COMMAND ""
)

ExternalProject_Add(blake3jni_arm64
  PREFIX blake3jni_arm64
  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmake/arm64"
  CMAKE_CACHE_ARGS -Darm64_toolchain:PATH=${CMAKE_SOURCE_DIR}/../../toolchain/aarch64-linux-gnu
                   -Drelease_dir:PATH=${release_directory}
  BUILD_ALWAYS 1
  INSTALL_COMMAND ""
)

ExternalProject_Add(blake3jni_x86
  PREFIX blake3jni_x86
  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmake/x86"
  CMAKE_CACHE_ARGS -Drelease_dir:PATH=${release_directory}
  BUILD_ALWAYS 1
  INSTALL_COMMAND ""
)

if (MSVC)
  ExternalProject_Add(blake3jni_android
    PREFIX blake3jni_android
    SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmake/android"
    CMAKE_CACHE_ARGS -Dndk:PATH=${CMAKE_SOURCE_DIR}/../../toolchain/android-ndk-r21e
    BUILD_ALWAYS 1
    INSTALL_COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/Release/android &&
                    ${CMAKE_COMMAND} -E copy "blake3_jni.dll" ${CMAKE_BINARY_DIR}/Release/android
  )
elseif(MSYS OR MINGW)
  ExternalProject_Add(blake3jni_android
    PREFIX blake3jni_android
    SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmake/android"
    CMAKE_CACHE_ARGS -Dndk:PATH=${CMAKE_SOURCE_DIR}/../../toolchain/android-ndk-r21e
    BUILD_ALWAYS 1
    INSTALL_COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/Release/android &&
                    ${CMAKE_COMMAND} -E copy "libblake3_jni.dll" ${CMAKE_BINARY_DIR}/Release/android
  )
elseif(UNIX)
  ExternalProject_Add(blake3jni_android
    PREFIX blake3jni_android
    SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmake/android"
    CMAKE_CACHE_ARGS -Dndk:PATH=${CMAKE_SOURCE_DIR}/../../toolchain/android-ndk-r21e
    BUILD_ALWAYS 1
    INSTALL_COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/Release/android &&
                    ${CMAKE_COMMAND} -E copy "libblake3_jni.so" ${CMAKE_BINARY_DIR}/Release/android
  )
endif()

add_dependencies(blake3jni_arm32 blake3_jni)
add_dependencies(blake3jni_arm64 blake3_jni)
add_dependencies(blake3jni_x86 blake3_jni)
add_dependencies(blake3jni_android blake3_jni)

add_dependencies(blake3_multiarch_libs blake3jni_arm32)
add_dependencies(blake3_multiarch_libs blake3jni_arm64)
add_dependencies(blake3_multiarch_libs blake3jni_x86)
add_dependencies(blake3_multiarch_libs blake3jni_android)

add_dependencies(blake3_arm64_libs blake3jni_arm64)
add_dependencies(blake3_arm32_libs blake3jni_arm32)
add_dependencies(blake3_android_libs blake3jni_android)
add_dependencies(blake3_x86_libs blake3jni_x86)

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
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/jni/java"
"${CMAKE_BINARY_DIR}/Release/headers"
    COMMENT "Making Release: Java Classes"
)


if(MSVC)
  add_compile_options(/I${_JAVA_HOME}\\include /I${_JAVA_HOME}\\include\\win32)
  add_compile_options(/nologo /O2 /W4 /wd4146 /wd4244)
endif()

if(MSYS OR MINGW)
  add_compile_options(-I${_JAVA_HOME}\\include -I${_JAVA_HOME}\\include\\win32)
  add_compile_options(-Wall -fPIC -Wextra -Wpedantic -Werror -Wno-array-bounds)
  add_compile_options(-Wmissing-prototypes -Wredundant-decls -Wshadow -Wpointer-arith)
  add_compile_options(-O3 -fomit-frame-pointer)
else()
  add_compile_options(-I${_JAVA_HOME}/include -I${_JAVA_HOME}/include/linux)
  add_compile_options(-Wall -fPIC -Wextra -Wpedantic -Werror -Wno-array-bounds)
  add_compile_options(-Wmissing-prototypes -Wredundant-decls -Wshadow -Wpointer-arith)
  add_compile_options(-O3 -fomit-frame-pointer)
endif()

if(MSVC)
    add_library(blake3_x86_jni SHARED ${BLAKE3_SRCS} ${BLAKE3_X86_MSVC_SRCS})
elseif(MSYS OR MINGW)
    add_library(blake3_x86_jni SHARED ${BLAKE3_SRCS} ${BLAKE3_X86_MINGW_SRCS})
elseif(UNIX)
    add_library(blake3_x86_jni SHARED ${BLAKE3_SRCS} ${BLAKE3_X86_UNIX_SRCS})
endif()

add_library(blake3_arm32_jni SHARED ${BLAKE3_SRCS})
add_library(blake3_arm64_jni SHARED ${BLAKE3_SRCS})

target_compile_definitions(blake3_arm64_jni PUBLIC BLAKE3_USE_NEON=1)

add_custom_command(
  TARGET blake3_x86_jni POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/Release/x86
)
if (MSVC)
  add_custom_command(
    TARGET blake3_x86_jni POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "blake3_x86_jni.dll" "${CMAKE_BINARY_DIR}/Release/x86"
    COMMENT "Making Release: blake3_x86_jni.dll"
  )
elseif(MSYS OR MINGW)
  add_custom_command(
    TARGET blake3_x86_jni POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "libblake3_x86_jni.dll" "${CMAKE_BINARY_DIR}/Release/x86"
    COMMENT "Making Release: libblake3_x86_jni.dll"
  )
elseif(UNIX)
  add_custom_command(
    TARGET blake3_x86_jni POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "libblake3_x86_jni.so" "${CMAKE_BINARY_DIR}/Release/x86"
    COMMENT "Making Release: libblake3_x86_jni.so"
  )
endif()

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
add_dependencies(blake3_arm32_jni blake3_jni)
add_dependencies(blake3_x86_jni blake3_jni)



if(MSVC)
    add_library(blake3_x86_jni SHARED ${BLAKE3_SRCS} ${BLAKE3_X86_MSVC_SRCS})
elseif(MSYS OR MINGW)
    add_library(blake3_x86_jni SHARED ${BLAKE3_SRCS} ${BLAKE3_X86_MINGW_SRCS})
elseif(UNIX)
    add_library(blake3_x86_jni SHARED ${BLAKE3_SRCS} ${BLAKE3_X86_UNIX_SRCS})
endif()

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

add_dependencies(blake3_x86_jni blake3_jni)


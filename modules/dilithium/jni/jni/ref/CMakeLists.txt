set(DILITHIUM_SRCS randombytes.c packing.c polyvec.c poly.c ntt.c reduce.c rounding.c)
set(DILITHIUM_FIPS202_SRCS sign.c ${DILITHIUM_SRCS} symmetric-shake.c)
set(DILITHIUM_AES_SRCS sign_aes.c ${DILITHIUM_SRCS} symmetric-aes.c)
set(FIPS202_SRCS fips202.c)
set(AES256CTR_SRCS aes256ctr.c)

find_package(Java)
find_package(Java COMPONENTS Development)

if(MSVC)
  add_compile_options(/I${_JAVA_HOME}\\include /I${_JAVA_HOME}\\include\\win32)
  add_compile_options(/nologo /O2 /W4 /wd4146 /wd4244)
endif()

if(MSYS OR MINGW)
  add_compile_options(-I${_JAVA_HOME}\\include -I${_JAVA_HOME}\\include\\win32)
  add_compile_options(-Wall -fPIC -Wextra -Wpedantic -Werror)
  add_compile_options(-Wmissing-prototypes -Wredundant-decls -Wshadow -Wpointer-arith)
  add_compile_options(-O3 -fomit-frame-pointer)
else()
  add_compile_options(-I${_JAVA_HOME}/include -I${_JAVA_HOME}/include/linux)
  add_compile_options(-Wall -fPIC -Wextra -Wpedantic -Werror)
  add_compile_options(-Wmissing-prototypes -Wredundant-decls -Wshadow -Wpointer-arith)
  add_compile_options(-O3 -fomit-frame-pointer)
endif()

add_library(fips202_ref_jni ${FIPS202_SRCS})
add_library(aes256ctr_ref_jni ${AES256CTR_SRCS})

# Dilithium 2
add_library(dilithium2_ref_jni SHARED ${DILITHIUM_FIPS202_SRCS} ${FIPS202_SRCS})
add_library(dilithium2aes_ref_jni SHARED ${DILITHIUM_AES_SRCS} ${FIPS202_SRCS} ${AES256CTR_SRCS})
target_compile_definitions(dilithium2_ref_jni PUBLIC DILITHIUM_MODE=2)
target_compile_definitions(dilithium2aes_ref_jni PUBLIC DILITHIUM_MODE=2 DILITHIUM_USE_AES)
target_link_libraries(dilithium2_ref_jni INTERFACE fips202_ref)
target_link_libraries(dilithium2aes_ref_jni INTERFACE fips202_ref aes256ctr_ref)

add_custom_command(
  TARGET dilithium2_ref_jni POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/Release
)
add_custom_command(
  TARGET dilithium2aes_ref_jni POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/Release/ref
)

if (MSVC)
  add_custom_command(
    TARGET dilithium2aes_ref_jni POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "dilithium2aes_ref_jni.dll" "${CMAKE_BINARY_DIR}/Release"
    COMMENT "Making Release: dilithium2aes_ref_jni.dll"
  )
endif()
if (MSYS OR MINGW)
  add_custom_command(
    TARGET dilithium2aes_ref_jni POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "libdilithium2aes_ref_jni.dll" "${CMAKE_BINARY_DIR}/Release"
    COMMENT "Making Release: libdilithium2aes_ref_jni.dll"
  )
endif()
if(UNIX)
  add_custom_command(
    TARGET dilithium2aes_ref_jni POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "libdilithium2aes_ref_jni.so" "${CMAKE_BINARY_DIR}/Release"
    COMMENT "Making Release: libdilithium2aes_ref_jni.so"
  )
endif()

if (MSVC)
  add_custom_command(
    TARGET dilithium2_ref_jni POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "dilithium2_ref_jni.dll" "${CMAKE_BINARY_DIR}/Release"
    COMMENT "Making Release: dilithium2_ref_jni.dll"
  )
endif()
if (MSYS OR MINGW)
  add_custom_command(
    TARGET dilithium2_ref_jni POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "libdilithium2_ref_jni.dll" "${CMAKE_BINARY_DIR}/Release"
    COMMENT "Making Release: libdilithium2_ref_jni.dll"
  )
endif()
if(UNIX)
  add_custom_command(
    TARGET dilithium2_ref_jni POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "libdilithium2_ref_jni.so" "${CMAKE_BINARY_DIR}/Release"
    COMMENT "Making Release: libdilithium2_ref_jni.so"
  )
endif()

add_custom_command(
    TARGET dilithium2_ref_jni POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/jni/java" "${CMAKE_BINARY_DIR}/Release/ref"
    COMMENT "Making Release: Java Classes"
)


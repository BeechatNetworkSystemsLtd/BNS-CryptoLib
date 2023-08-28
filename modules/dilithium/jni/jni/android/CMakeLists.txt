set(DILITHIUM_SRCS ../ref/randombytes.c ../ref/packing.c ../ref/polyvec.c ../ref/poly.c ../ref/ntt.c ../ref/reduce.c ../ref/rounding.c)
set(DILITHIUM_FIPS202_SRCS ../ref/sign.c ${DILITHIUM_SRCS} ../ref/symmetric-shake.c)
set(DILITHIUM_AES_SRCS ../ref/sign_aes.c ${DILITHIUM_SRCS} ../ref/symmetric-aes.c)
set(FIPS202_SRCS ../ref/fips202.c)
set(AES256CTR_SRCS ../ref/aes256ctr.c)


set(CMAKE_C_COMPILER
../../../../../../toolchain/android-ndk-r21e/toolchains/llvm/prebuilt/linux-x86_64/bin/armv7a-linux-androideabi29-clang)
set(CMAKE_SYSROOT ../../../../../../toolchain/android-ndk-r21e/toolchains/llvm/prebuilt/linux-x86_64/sysroot)

if(MSYS OR MINGW)
else()
    add_compile_options(-fPIC -shared -std=c99 -o build/apk/lib/arm64-v8a/libdilithium2_ref_jni.so)
    add_compile_options(-D DILITHIUM_MODE=2)
    add_compile_options(-march=armv7-a)
endif()

add_library(dilithium2_ref_jni_android SHARED ${DILITHIUM_FIPS202_SRCS} ${FIPS202_SRCS})
add_library(dilithium2aes_ref_jni_android SHARED ${DILITHIUM_AES_SRCS} ${FIPS202_SRCS} ${AES256CTR_SRCS})

target_compile_definitions(dilithium2aes_ref_jni_android PUBLIC DILITHIUM_USE_AES)
add_custom_command(
  TARGET dilithium2aes_ref_jni_android POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/Release/android
)

if (MSVC)
  add_custom_command(
    TARGET dilithium2_ref_jni_android POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "dilithium2_ref_jni_android.dll" "${CMAKE_BINARY_DIR}/Release/android"
    COMMENT "Making Release: dilithium2_ref_jni_android.dll"
  )
endif()
if (MSYS OR MINGW)
  add_custom_command(
    TARGET dilithium2_ref_jni_android POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "libdilithium2_ref_jni_android.dll" "${CMAKE_BINARY_DIR}/Release/android"
    COMMENT "Making Release: libdilithium2_ref_jni_android.dll"
  )
endif()
if(UNIX)
  add_custom_command(
    TARGET dilithium2_ref_jni_android POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "libdilithium2_ref_jni_android.so" "${CMAKE_BINARY_DIR}/Release/android"
    COMMENT "Making Release: libdilithium2_ref_jni_android.so"
  )
endif()


if (MSVC)
  add_custom_command(
    TARGET dilithium2_ref_jni_android POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "dilithium2aes_ref_jni_android.dll" "${CMAKE_BINARY_DIR}/Release/android"
    COMMENT "Making Release: dilithium2aes_ref_jni_android.dll"
  )
endif()
if (MSYS OR MINGW)
  add_custom_command(
    TARGET dilithium2_ref_jni_android POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "libdilithium2aes_ref_jni_android.dll" "${CMAKE_BINARY_DIR}/Release/android"
    COMMENT "Making Release: libdilithium2aes_ref_jni_android.dll"
  )
endif()
if(UNIX)
  add_custom_command(
    TARGET dilithium2_ref_jni_android POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "libdilithium2aes_ref_jni_android.so" "${CMAKE_BINARY_DIR}/Release/android"
    COMMENT "Making Release: libdilithium2aes_ref_jni_android.so"
  )
endif()


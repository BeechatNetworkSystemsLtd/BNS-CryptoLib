set(KYBER_SRCS kex.c kem.c indcpa.c polyvec.c poly.c ntt.c cbd.c reduce.c verify.c)
set(KYBER_FIPS202_SRCS ${KYBER_SRCS} symmetric-shake.c)
set(KYBER_NINETIES_SRCS ${KYBER_SRCS} symmetric-aes.c)
set(FIPS202_SRCS fips202.c)
set(AES256CTR_SRCS aes256ctr.c)
set(SHA2_SRCS sha256.c sha512.c)
set(TEST_KYBER_SRCS test_kyber.c randombytes.c)
set(TEST_KEX_SRCS test_kex.c randombytes.c)
set(TEST_VECTORS_SRCS test_vectors.c)
set(TEST_SPEED_SRCS test_speed.c speed_print.c cpucycles.c randombytes.c)

if(MSVC)
  add_compile_options(/nologo /O2 /W4 /wd4146 /wd4244)
else()
  add_compile_options(-Wall -Wextra -Wpedantic -Werror)
  add_compile_options(-Wmissing-prototypes -Wredundant-decls -Wshadow -Wpointer-arith)
  add_compile_options(-O3 -fomit-frame-pointer)
endif()

add_library(fips202_ref ${FIPS202_SRCS})
add_library(aes256ctr_ref ${AES256CTR_SRCS})
add_library(sha2_ref ${SHA2_SRCS})

# Kyber 512
add_library(kyber512_ref ${KYBER_FIPS202_SRCS})
add_library(kyber512_90s_ref ${KYBER_NINETIES_SRCS})
target_compile_definitions(kyber512_ref PUBLIC KYBER_K=2)
target_compile_definitions(kyber512_90s_ref PUBLIC KYBER_K=2 KYBER_90S)
target_link_libraries(kyber512_ref INTERFACE fips202_ref)
target_link_libraries(kyber512_90s_ref INTERFACE aes256ctr_ref sha2_ref)

add_executable(test_kyber512_ref ${TEST_KYBER_SRCS})
add_executable(test_kex512_ref ${TEST_KEX_SRCS})
add_executable(test_vectors512_ref ${TEST_VECTORS_SRCS})
add_executable(test_kyber512-90s_ref ${TEST_KYBER_SRCS})
add_executable(test_kex512-90s_ref ${TEST_KEX_SRCS})
add_executable(test_vectors512-90s_ref ${TEST_VECTORS_SRCS})
add_executable(test_speed ${TEST_SPEED_SRCS})

option(VERSION32 "Aarch32 build" OFF)
option(VERSION64 "Aarch32 build" OFF)
IF (VERSION32)
  message("Aarch32 build")
  target_compile_definitions(test_speed PUBLIC VERSION32)
ENDIF()
IF (VERSION64)
  message("Aarch64 build")
  target_compile_definitions(test_speed PUBLIC VERSION64)
ENDIF()

target_compile_options(test_speed PUBLIC -static)

target_link_libraries(test_kyber512_ref kyber512_ref)
target_link_libraries(test_kex512_ref kyber512_ref)
target_link_libraries(test_vectors512_ref kyber512_ref)
target_link_libraries(test_kyber512-90s_ref kyber512_90s_ref)
target_link_libraries(test_kex512-90s_ref kyber512_90s_ref)
target_link_libraries(test_vectors512-90s_ref kyber512_90s_ref)
target_link_libraries(test_speed kyber512_ref)

add_custom_command(
  TARGET test_speed POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/ref/speed_test
)
add_custom_command(
  TARGET test_speed POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/ref/speed_test ${CMAKE_BINARY_DIR}/ref/speed_test
  COMMENT "Making Release: speed_test.tar.gz"
)
IF (VERSION32)
  add_custom_command(
    TARGET test_speed POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/ref/speed_test/clock_module/clock_enabler64.c
  )
ENDIF()
IF (VERSION64)
  add_custom_command(
    TARGET test_speed POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/ref/speed_test/clock_module/clock_enabler.c
  )
  add_custom_command(
    TARGET test_speed POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E rename ${CMAKE_BINARY_DIR}/ref/speed_test/clock_module/clock_enabler64.c ${CMAKE_BINARY_DIR}/ref/speed_test/clock_module/clock_enabler.c
  )
ENDIF()

add_custom_command(
  TARGET test_speed POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/ref/test_speed ${CMAKE_BINARY_DIR}/ref/speed_test
)
add_custom_target(create_tar ALL COMMAND
  ${CMAKE_COMMAND} -E tar "czvf" ${CMAKE_BINARY_DIR}/speed_test.tar.gz speed_test
  COMMENT "Done: speed_test.tar.gz"
)

add_test(NAME kyber512_ref COMMAND test_kyber512_ref)
add_test(NAME kex512_ref COMMAND test_kex512_ref)
add_test(NAME kyber512-90s_ref COMMAND test_kyber512-90s_ref)
add_test(NAME kex512-90_ref COMMAND test_kex512-90s_ref)

if(WIN32)
  add_test(NAME vectors512_ref COMMAND PowerShell -Command "$<TARGET_FILE:test_vectors512_ref> | dos2unix > tvecs512")
  add_test(NAME vectors512-90s_ref COMMAND PowerShell -Command "$<TARGET_FILE:test_vectors512-90s_ref> | dos2unix > tvecs512-90s")
else()
  add_test(NAME vectors512_ref COMMAND sh -c "\"$<TARGET_FILE:test_vectors512_ref>\" > tvecs512")
  add_test(NAME vectors512-90s_ref COMMAND sh -c "\"$<TARGET_FILE:test_vectors512-90s_ref>\" > tvecs512-90s")
endif()

add_test(NAME hashes COMMAND sha256sum -c ../../SHA256SUMS)

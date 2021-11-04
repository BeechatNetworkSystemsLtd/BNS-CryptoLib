cmake_minimum_required(VERSION 3.10)
project(pykyber C CXX ASM)

# Tell cmake that headers are in alse in source_dir
set(SOURCE_DIR "src/ref")
include_directories(${SOURCE_DIR})

set(KYBER_SRCS
    ${SOURCE_DIR}/kex.c
    ${SOURCE_DIR}/kem.c
    ${SOURCE_DIR}/indcpa.c
    ${SOURCE_DIR}/polyvec.c
    ${SOURCE_DIR}/poly.c
    ${SOURCE_DIR}/ntt.c
    ${SOURCE_DIR}/cbd.c
    ${SOURCE_DIR}/reduce.c
    ${SOURCE_DIR}/verify.c
    ${SOURCE_DIR}/randombytes.c
    ${SOURCE_DIR}/fips202.c
    ${SOURCE_DIR}/sha256.c
    ${SOURCE_DIR}/sha512.c
)
set(KYBER_FIPS202_SRCS ${KYBER_SRCS} ${SOURCE_DIR}/symmetric-shake.c)
set(KYBER_NINETIES_SRCS ${KYBER_SRCS} ${SOURCE_DIR}/symmetric-aes.c)
set(FIPS202_SRCS ${SOURCE_DIR}/fips202.c)
set(AES256CTR_SRCS ${SOURCE_DIR}/aes256ctr.c)
set(SHA2_SRCS ${SOURCE_DIR}/sha256.c ${SOURCE_DIR}/sha512.c)

if(MSVC)
  add_compile_options(/nologo /O2 /W4 /wd4146 /wd4244)
else()
  add_compile_options(-Wall -Wextra -Wpedantic -Werror)
  add_compile_options( -Wredundant-decls -Wshadow -Wpointer-arith)
  add_compile_options(-O3 -fomit-frame-pointer)
endif()

add_library(fips202_ref ${FIPS202_SRCS})
add_library(aes256ctr_ref ${AES256CTR_SRCS})
add_library(sha2_ref ${SHA2_SRCS})


# Generate python module
add_subdirectory(lib/pybind11)

# Kyber 512
pybind11_add_module(pykyber ${KYBER_FIPS202_SRCS} "${SOURCE_DIR}/base58.c" "${SOURCE_DIR}/bindings.cpp")
pybind11_add_module(pykyber_90s ${KYBER_NINETIES_SRCS} ${AES256CTR_SRCS} ${SHA2_SRCS} "${SOURCE_DIR}/base58.c" "${SOURCE_DIR}/bindings_90s.cpp")

target_compile_definitions(pykyber PUBLIC KYBER_K=2)
target_compile_definitions(pykyber_90s PUBLIC KYBER_K=2 KYBER_90S)
target_link_libraries(pykyber INTERFACE fips202_ref)
target_link_libraries(pykyber_90s INTERFACE aes256ctr_ref sha2_ref)



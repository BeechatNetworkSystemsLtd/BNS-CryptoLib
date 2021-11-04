cmake_minimum_required(VERSION 3.10)
project(pydilithium C CXX ASM)

# Tell cmake that headers are in alse in source_dir
set(SOURCE_DIR "src/ref")
include_directories(${SOURCE_DIR})

set(DILITHIUM_SRCS
    ${SOURCE_DIR}/sign.c
    ${SOURCE_DIR}/packing.c
    ${SOURCE_DIR}/polyvec.c
    ${SOURCE_DIR}/poly.c
    ${SOURCE_DIR}/ntt.c
    ${SOURCE_DIR}/reduce.c
    ${SOURCE_DIR}/rounding.c
    ${SOURCE_DIR}/randombytes.c
    ${SOURCE_DIR}/fips202.c
)
set(DILITHIUM_FIPS202_SRCS ${DILITHIUM_SRCS} ${SOURCE_DIR}/symmetric-shake.c)
set(DILITHIUM_NINETIES_SRCS ${DILITHIUM_SRCS} ${SOURCE_DIR}/symmetric-aes.c)
set(FIPS202_SRCS ${SOURCE_DIR}/fips202.c)
set(AES256CTR_SRCS ${SOURCE_DIR}/aes256ctr.c)

if(MSVC)
  add_compile_options(/nologo /O2 /W4 /wd4146 /wd4244)
else()
  add_compile_options(-Wall -Wextra -Wpedantic -Werror)
  add_compile_options( -Wredundant-decls -Wshadow -Wpointer-arith)
  add_compile_options(-O3 -fomit-frame-pointer)
endif()

add_library(fips202_ref ${FIPS202_SRCS})
add_library(aes256ctr_ref ${AES256CTR_SRCS})

# Generate python module
add_subdirectory(lib/pybind11)

# Dilithium 2
pybind11_add_module(pydilithium ${DILITHIUM_FIPS202_SRCS} "${SOURCE_DIR}/base58.c" "${SOURCE_DIR}/bindings.cpp")
pybind11_add_module(pydilithium_aes ${DILITHIUM_NINETIES_SRCS} ${AES256CTR_SRCS} ${SHA2_SRCS} "${SOURCE_DIR}/base58.c" "${SOURCE_DIR}/bindings_aes.cpp")
target_compile_definitions(pydilithium PUBLIC DILITHIUM_MODE=2)
target_compile_definitions(pydilithium_aes PUBLIC DILITHIUM_MODE=2 DILITHIUM_USE_AES)
target_link_libraries(pydilithium INTERFACE fips202_ref)
target_link_libraries(pydilithium_aes INTERFACE aes256ctr_ref sha2_ref)



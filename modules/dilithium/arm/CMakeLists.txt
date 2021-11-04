cmake_minimum_required(VERSION 3.10)
SET(CMAKE_SYSTEM_NAME Linux)

# Set ARM compiler
SET(CMAKE_FIND_ROOT_PATH ${TOOLS_PATH})

project(dilithium C ASM)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

enable_testing()

add_subdirectory(ref)

cmake_minimum_required(VERSION 3.10)

project(blake3jni C ASM)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

enable_testing()

add_subdirectory(jni)


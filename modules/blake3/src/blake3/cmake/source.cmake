set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(BLAKE3_SRCS
    ../../blake3.c
    ../../network_beechat_Blake3.c
    ../../blake3_dispatch.c
    ../../blake3_portable.c
)
set(BLAKE3_X86_UNIX_SRCS
    ../../blake3_sse2_x86-64_unix.S
    ../../blake3_sse41_x86-64_unix.S
    ../../blake3_avx2_x86-64_unix.S
    ../../blake3_avx512_x86-64_unix.S
)
set(BLAKE3_X86_MSVC_SRCS
    ../../blake3_sse2_x86-64_windows_msvc.asm
    ../../blake3_sse41_x86-64_windows_msvc.asm
    ../../blake3_avx2_x86-64_windows_msvc.asm
    ../../blake3_avx512_x86-64_windows_msvc.asm
)
set(BLAKE3_X86_MINGW_SRCS
    ../../blake3_sse2_x86-64_windows_gnu.S
    ../../blake3_sse41_x86-64_windows_gnu.S
    ../../blake3_avx2_x86-64_windows_gnu.S
    ../../blake3_avx512_x86-64_windows_gnu.S
)



#!/bin/sh -e

ARCH="${ARCH:-arm64}"
ARCH="${TRAVIS_CPU_ARCH:-$ARCH}"

DIRS="build/ref"

export CFLAGS="-fsanitize=address,undefined ${CFLAGS}"

for dir in $DIRS; do
  for alg in 512_ref 512-90s_ref; do
    #valgrind --vex-guest-max-insns=25 ./$dir/test_kyber$alg
    ./$dir/test_kyber$alg &
    PID1=$!
    ./$dir/test_kex$alg &
    PID2=$!
    ./$dir/test_vectors$alg > tvecs$alg &
    PID3=$!
    wait $PID1 $PID2 $PID3
  done
  shasum -a256 -c SHA256SUMS
done

exit 0

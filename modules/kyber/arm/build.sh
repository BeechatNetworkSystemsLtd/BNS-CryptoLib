#!/bin/bash

rm -rf build
mkdir build
if [ "$1" == "-b=64" ]; then
    echo "Building x64-bit application"
    export TOOLS_PATH=$(pwd)/../../../toolchain/aarch64-linux-gnu
    export CC=$TOOLS_PATH/bin/aarch64-linux-gnu-gcc
    cd build
    cmake -DVERSION64=ON ..
else
    echo "Building x32-bit application"
    export TOOLS_PATH=$(pwd)/../../../toolchain/arm-linux-gnueabihf
    export CC=$TOOLS_PATH/bin/arm-linux-gnueabihf-gcc
    cd build
    cmake -DVERSION32=ON ..
fi
make


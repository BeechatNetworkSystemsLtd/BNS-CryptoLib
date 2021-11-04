#!/bin/bash

mkdir -p build-blake
cd build-blake
cmake ../modules/blake3
make -j`nproc`
cd ..

rm -rf build-kyber/arm
mkdir -p build-kyber/arm
cd modules/kyber/arm
./build.sh -b=64
cd ../../..
mv modules/kyber/arm/build build-kyber/arm/build64
cd modules/kyber/arm
./build.sh -b=32
cd ../../..
mv modules/kyber/arm/build build-kyber/arm/build32

mkdir -p build-kyber/jni
cd build-kyber/jni
cmake ../../modules/kyber/jni
make -j`nproc`
cd ../../

mkdir -p build-kyber/py
cd build-kyber/py
cmake ../../modules/kyber/py
make -j`nproc`
cd ../../

rm -rf build-dilithium/arm
mkdir -p build-dilithium/arm
cd modules/dilithium/arm
./build.sh -b=64
cd ../../..
mv modules/dilithium/arm/build build-dilithium/arm/build64
cd modules/dilithium/arm
./build.sh -b=32
cd ../../..
mv modules/dilithium/arm/build build-dilithium/arm/build32

mkdir -p build-dilithium/jni
cd build-dilithium/jni
cmake ../../modules/dilithium/jni
make -j`nproc`
cd ../../

mkdir -p build-dilithium/py
cd build-dilithium/py
cmake ../../modules/dilithium/py
make -j`nproc`
cd ../../


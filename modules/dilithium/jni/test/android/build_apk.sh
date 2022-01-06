#!/bin/bash

HOME="../../../../../toolchain"
SDK="${HOME}/android-sdk-linux"
BUILD_TOOLS="${SDK}/build-tools/25.0.0"
PLATFORM="${SDK}/platforms/android-21"
NDK="${HOME}/android-ndk-r13b"
ARM_TOOLCHAIN="${NDK}/toolchains/aarch64-linux-android-4.9/prebuilt/"
ARM_TOOLCHAIN+="linux-x86_64/bin/aarch64-linux-android-gcc"
REF_SRC="../../jni/ref"

rm -rf build

mkdir -p build/gen build/obj build/apk
mkdir -p build/apk/lib/arm64-v8a


"${ARM_TOOLCHAIN}" --sysroot="${NDK}/platforms/android-21/arch-arm64" \
    -march=armv8-a \
    -D DILITHIUM_MODE=2 \
    -fPIC -shared -std=c99 -o build/apk/lib/arm64-v8a/libdilithium2_ref_jni.so \
    ${REF_SRC}/sign.c \
    ${REF_SRC}/symmetric-shake.c \
    ${REF_SRC}/randombytes.c \
    ${REF_SRC}/packing.c \
    ${REF_SRC}/polyvec.c \
    ${REF_SRC}/poly.c \
    ${REF_SRC}/ntt.c \
    ${REF_SRC}/reduce.c \
    ${REF_SRC}/fips202.c \
    ${REF_SRC}/rounding.c

"${ARM_TOOLCHAIN}" --sysroot="${NDK}/platforms/android-21/arch-arm64" \
    -march=armv8-a \
    -D DILITHIUM_MODE=2 \
    -D DILITHIUM_USE_AES \
    -fPIC -shared -std=c99 -o build/apk/lib/arm64-v8a/libdilithium2aes_ref_jni.so \
    ${REF_SRC}/sign_aes.c \
    ${REF_SRC}/symmetric-shake.c \
    ${REF_SRC}/randombytes.c \
    ${REF_SRC}/packing.c \
    ${REF_SRC}/polyvec.c \
    ${REF_SRC}/poly.c \
    ${REF_SRC}/ntt.c \
    ${REF_SRC}/reduce.c \
    ${REF_SRC}/fips202.c \
    ${REF_SRC}/rounding.c \
    ${REF_SRC}/symmetric-aes.c \
    ${REF_SRC}/aes256ctr.c


"${BUILD_TOOLS}/aapt" package -f -m -J build/gen/ -S res \
    -M AndroidManifest.xml -I "${PLATFORM}/android.jar"

javac -source 1.7 -target 1.7 -bootclasspath "${JAVA_HOME}/jre/lib/rt.jar" \
    -classpath "${PLATFORM}/android.jar" -d build/obj \
    build/gen/net/test/R.java java/net/test/*.java ../../jni/java/*.java

"${BUILD_TOOLS}/dx" --dex --output=build/apk/classes.dex build/obj/

"${BUILD_TOOLS}/aapt" package -f -M AndroidManifest.xml -S res/ \
    -I "${PLATFORM}/android.jar" \
    -F build/DilithiumTest.unsigned.apk build/apk/

"${BUILD_TOOLS}/zipalign" -f -p 4 \
    build/DilithiumTest.unsigned.apk build/DilithiumTest.aligned.apk


"${BUILD_TOOLS}/apksigner" sign --ks keystore.jks \
    --ks-key-alias androidkey --ks-pass pass:android \
    --key-pass pass:android --out build/DilithiumTest.apk \
    build/DilithiumTest.aligned.apk


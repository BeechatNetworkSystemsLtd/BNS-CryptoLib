# Beechat Network CryptoLib

This repository contains the reference implementation of several cryptographic tools.

## Build

```sh
./build.sh
```
The build result will be available in the `/build-*` directories.

## Tools

* [Kyber-ARM](modules/kyber/arm) - Kyber ARM version
* [Kyber-JNI](modules/kyber/jni) - Kyber JNI native wrappers
* [Kyber-Py](modules/kyber/py) - Kyber pybind11 wrappers
* [Dilithium-ARM](modules/dilithium/arm) - Dilithium ARM version
* [Dilithium-JNI](modules/dilithium/jni) - Dilithium JNI native wrappers
* [Dilithium-Py](modules/dilithium/py) - Dilithium pybind11 wrappers
* [Blake3](modules/blake3) - BLAKE3 JNI + Android + pybind11 wrappers (x86, arm32, aarch64)


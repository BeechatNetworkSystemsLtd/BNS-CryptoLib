# Dilithium-JNI

This repository contains the reference implementation of the [Dilithium](https://github.com/pq-crystals/dilithium/) key encapsulation mechanism.

## Build

For make a JNI wrapper you should use cmake:
```sh
mkdir build
cd build
cmake ..
make
```
The build result will be available in the `/build/Release` directory.
Or you can just open the project in Visual Studio and build it inside the IDE.

## Usage

To use the Kyber functions in a JAVA project, you must use the classes located in the `/build/Release/ref` directory.
By default, everything is packaged in the network package.beuchat; If you want to change the package name, you need to change the headers (`ssign.h` and `ssign_aes.h`) in `jni/ref` directory.

Insert JNI classes into your application and use necessary functions. For example:

```java
import network.beechat.*;
...
public static void main(String[] args) {
    char[] pk = new char[32];
    char[] sk = new char[32];
    int rc = Dilithium.crypto_kem_keypair(pk, sk);
    System.exit(rc);
}
```
When importing classes (from `jni/java`), you can change the path to the dynamic library. By default, it is set to the current startup directory. Do not forget about this parameter, because it depends on place of the library.

```java
static {
    System.loadLibrary("kyber512_90s_ref_jni");
}
```

The necessary shared libraries are placed in `/build/Release` directory. It is not necessary to use all classes (with `_aes` and without). Just choose what you want to use.


## For Android Test

Paste this commands:

```bash
cd test/android
./build_apk.sh
```

Result APK will be test/android/build/DilithiumTest.apk.
Just install and run.


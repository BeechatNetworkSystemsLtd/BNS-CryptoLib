# Blake3-JNI

This repository contains the reference implementation of the [Blake3](https://github.com/BLAKE3-team/BLAKE3) hash algorithm usable through Java Native Interface wrappers for x86 and ARM.

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

To use the Blake3 functions in a JAVA project, you must use the classes located in the
`/build/Release/headers` directory.
By default, everything is packaged in the network.beechat package; If you want to change
the package name, you need to change the header (`network_beechat_Blake3.h`) in `src/blake3` directory.

Insert JNI classes into your application and use necessary functions. For example:

```java
import network.beechat.*;
...
public static void main(String[] args) {
    char[] pk = new char[32];
    char[] sk = new char[32];
    int rc = Kyber512.crypto_kem_keypair(pk, sk);
    System.exit(rc);
}
```
When importing classes (from `jni/java`), you can change the path to the dynamic library. By default, it is set to the current startup directory. Do not forget about this parameter, because it depends on place of the library.

Example for Windows:
```java
static {
    System.loadLibrary("blake3_x86_jni.dll");
}
```

Example for Unix-like:
```java
static {
    System.loadLibrary("libblake3_x86_jni.so");
}
```

The necessary shared libraries are placed in `/build/Release` directory for each supported architecture.


## For Android Test

Paste this commands:

```bash
cd test/android
./build_apk.sh
```

Result APK will be test/android/build/Blake3Test.apk.
Just install and run.


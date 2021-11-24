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
This is the general build procedure but you can use a specific targets not included in
general build. Any builds will be available in the `/build/Release` directory.


### Build targets

| Target      | Description                | Result (build/Release) |
|:------------|:--------------------------:| ----------------------:|
| arm32       | ARM32 JNI library version  | arm32/blake3_jni.so    |
| arm64       | ARM64 JNI library version  | arm64/blake3_jni.so    |
| x86         | x86_64 JNI library version | x86/blake3_jni.so      |
| android     | Android spec. build        | android/blake3_jni.so  |
| x86test     | Comparative analysis       | diagram.png            |
| androidtest | Comparative analysis       | Blake3Test.apk         |



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

Example:
```java
static {
    System.loadLibrary("blake3_jni");
}
```

The necessary shared libraries are placed in `/build/Release` directory for each supported architecture.


## For Android Test

Paste this commands:

```bash
cd test/android
./build_apk.sh
```

Result APK will be build/Release/Blake3Test.apk.
Just install and run.


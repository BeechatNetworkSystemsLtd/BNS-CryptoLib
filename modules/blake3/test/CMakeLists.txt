find_package(Java REQUIRED)
find_package(Java COMPONENTS Development)

include(UseJava)

set(SOURCE
    network/beechat/Blake3JNIx86Test.java
    ../src/java/Blake3.java
)

set(BUILD_TOOLS ${CMAKE_SOURCE_DIR}/../../toolchain/android-sdk-linux/build-tools/25.0.0)
set(PLATFORMM ${CMAKE_SOURCE_DIR}/../../toolchain/android-sdk-linux/platforms/android-21)

add_jar(Blake3Test SOURCES ${SOURCE} MANIFEST manifest.mf)

add_custom_target(
    runBlake3Test
    COMMAND java -Djava.library.path=${CMAKE_BINARY_DIR}/Release/x86 -jar ${CMAKE_BINARY_DIR}/test/Blake3Test.jar
)

add_custom_command(
    TARGET runBlake3Test POST_BUILD
    COMMAND pip3 install numpy && pip3 install pandas && pip3 install matplotlib && python3 ${CMAKE_SOURCE_DIR}/test/diagram.py
    COMMENT "Diagram generating"
)
add_dependencies(runBlake3Test blake3_x86_libs)
add_dependencies(runBlake3Test Blake3Test)

add_custom_target(blake3AndroidTest)

add_custom_command(
    TARGET blake3AndroidTest POST_BUILD
    COMMAND rm -rf abuild &&
            mkdir -p abuild/gen abuild/obj abuild/apk abuild/apk/lib/arm64-v8a
    COMMENT "Preparing android build ..."
)

add_custom_command(
    TARGET blake3AndroidTest POST_BUILD
    COMMAND cp ${CMAKE_BINARY_DIR}/Release/android/libblake3_jni.so abuild/apk/lib/arm64-v8a
    COMMENT "Copy Blake3 library ..."
)

add_custom_command(
    TARGET blake3AndroidTest POST_BUILD
    COMMAND ${BUILD_TOOLS}/aapt package -f -m -J abuild/gen/
        -S ${CMAKE_SOURCE_DIR}/test/android/res
        -M ${CMAKE_SOURCE_DIR}/test/android/AndroidManifest.xml
        -I ${PLATFORMM}/android.jar
    COMMENT "Prepare android.jar"
)

add_custom_command(
    TARGET blake3AndroidTest POST_BUILD
    COMMAND javac -source 1.7 -target 1.7
        -bootclasspath ${_JAVA_HOME}/jre/lib/rt.jar
        -classpath ${PLATFORMM}/android.jar -d abuild/obj abuild/gen/net/test/R.java
        ${CMAKE_SOURCE_DIR}/test/android/java/net/test/*.java
        ${CMAKE_SOURCE_DIR}/src/java/Blake3.java
    COMMENT "Compile ..."
)

add_custom_command(
    TARGET blake3AndroidTest POST_BUILD
    COMMAND ${BUILD_TOOLS}/dx --dex --output=abuild/apk/classes.dex abuild/obj/
    COMMENT "Making DEX ..."
)

add_custom_command(
    TARGET blake3AndroidTest POST_BUILD
    COMMAND ${BUILD_TOOLS}/aapt package -f -M
        ${CMAKE_SOURCE_DIR}/test/android/AndroidManifest.xml -S
        ${CMAKE_SOURCE_DIR}/test/android/res/ -I ${PLATFORMM}/android.jar
        -F abuild/Blake3Test.unsigned.apk abuild/apk/
    COMMENT "Packing DEX ..."
)

add_custom_command(
    TARGET blake3AndroidTest POST_BUILD
    COMMAND ${BUILD_TOOLS}/zipalign -f -p 4 abuild/Blake3Test.unsigned.apk
        abuild/Blake3Test.aligned.apk
    COMMENT "ZIP align ..."
)

add_custom_command(
    TARGET blake3AndroidTest POST_BUILD
    COMMAND ${BUILD_TOOLS}/apksigner sign --ks ${CMAKE_SOURCE_DIR}/test/android/keystore.jks
        --ks-key-alias androidkey --ks-pass pass:android
        --key-pass pass:android --out abuild/Blake3Test.apk
        abuild/Blake3Test.aligned.apk

    COMMENT "ZIP align ..."
)

add_custom_command(
    TARGET blake3AndroidTest POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy abuild/Blake3Test.apk ${CMAKE_BINARY_DIR}/Release
    COMMENT "Making Release Blake3Test.apk ..."
)

add_custom_target(blake3Arm64Test)

add_custom_command(
    TARGET blake3Arm64Test POST_BUILD
    COMMAND mkdir -p ${CMAKE_BINARY_DIR}/Release/temp
    COMMENT "Making a temp dir for arm64 test ..."
)
add_custom_command(
    TARGET blake3Arm64Test POST_BUILD
    COMMAND cp ${CMAKE_SOURCE_DIR}/test/jdk.tar.gz ${CMAKE_BINARY_DIR}/Release/temp
    COMMENT "Install arm64 JDK ..."
)
add_custom_command(
    TARGET blake3Arm64Test POST_BUILD
    COMMAND cp Blake3Test.jar ${CMAKE_BINARY_DIR}/Release/temp
    COMMENT "Install a test JAR ..."
)
add_custom_command(
    TARGET blake3Arm64Test POST_BUILD
    COMMAND cp ${CMAKE_BINARY_DIR}/Release/arm64/libblake3_jni.so ${CMAKE_BINARY_DIR}/Release/temp
    COMMENT "Install a lib ..."
)
add_custom_command(
    TARGET blake3Arm64Test POST_BUILD
    COMMAND cd ${CMAKE_BINARY_DIR}/Release && tar -czvf arm64_test.tar.gz temp
    COMMENT "Compressing an archive ..."
)
add_custom_command(
    TARGET blake3Arm64Test POST_BUILD
    COMMAND rm -rf ${CMAKE_BINARY_DIR}/Release/temp
)

add_dependencies(blake3Arm64Test blake3_arm64_libs)
add_dependencies(blake3Arm64Test Blake3Test)
add_dependencies(blake3AndroidTest blake3_android_libs)


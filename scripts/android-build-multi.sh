#!/usr/bin/env bash
#
# Android multi-arch apk/aab build script
#
# the following enviroment variables must be set
#
# ANDROID_SDK_ROOT               folder of the Android SDK
# ANDROID_NDK_ROOT               folder of the Android NDK - min version 25.1
# JAVA_HOME                      folder of the Java JDK - min version 11
#
# Qt_HOST_DIR                    folder containing Qt for the Linux host
# Qt_armeabi_v7a_DIR             folder containing Qt for Android arm 32 bit
# Qt_arm64_v8a_DIR               folder containing Qt for Android arm 64 bit
# Qt_x86_DIR                     folder containing Qt for Android x86
# Qt_x86_64_DIR                  folder containing Qt for Android x86_64
#
# QT_ANDROID_KEYSTORE_PATH       path to the keystore file to use for signing
# QT_ANDROID_KEYSTORE_ALIAS      keystore name alias
# QT_ANDROID_KEYSTORE_STORE_PASS keystore password
# QT_ANDROID_KEYSTORE_KEY_PASS   keystore password
#
# VERSION                        application version

set -e

build_dir="build-android-release"

cmake -B $build_dir \
    -D CMAKE_BUILD_TYPE=Release \
    -D CMAKE_TOOLCHAIN_FILE="$Qt_armeabi_v7a_DIR/lib/cmake/Qt6/qt.toolchain.cmake" \
    -D QT_ANDROID_ABIS="armeabi-v7a;arm64-v8a;x86;x86_64" \
    -D QT_ANDROID_SIGN_APK=Yes \
    -D QT_ANDROID_SIGN_AAB=Yes \
    -D QT_HOST_PATH="$Qt_HOST_DIR" \
    -D QT_PATH_ANDROID_ABI_armeabi-v7a="$Qt_armeabi_v7a_DIR" \
    -D QT_PATH_ANDROID_ABI_arm64-v8a="$Qt_arm64_v8a_DIR" \
    -D QT_PATH_ANDROID_ABI_x86="$Qt_x86_DIR" \
    -D QT_PATH_ANDROID_ABI_x86_64="$Qt_x86_64_DIR"
cmake -B $build_dir

cmake --build $build_dir --parallel 2 -- apk
cmake --build $build_dir --parallel 2 -- aab

mv $build_dir/android-build/tflite-examples.apk tflite-examples_${VERSION}.apk
mv $build_dir/android-build/tflite-examples.aab tflite-examples_${VERSION}.aab

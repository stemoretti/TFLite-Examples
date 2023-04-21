#!/usr/bin/env bash
#
# Android single-arch apk build script
#
# This script should be run with the Android arch given as argument.
# Android arch is one of armeabi-v7a (default), arm64-v8a, x86 and x86_64.
#
# The following environment variables must be set:
#
# ANDROID_SDK_ROOT     folder of the Android SDK
# ANDROID_NDK_ROOT     folder of the Android NDK - min version 23.1
# JAVA_HOME            folder of the Java JDK - min version 11
#
# Qt_HOST_DIR          folder containing Qt for the Linux host
# Qt_{arch}_DIR        folder containing Qt for Android
#
# VERSION              application version

set -e

arch=${1:-armeabi-v7a}
arch_path=$(eval "echo \$Qt_${arch/-/_}_DIR")
build_dir="build-$arch"

cmake -B $build_dir \
    -D CMAKE_BUILD_TYPE=Release \
    -D CMAKE_TOOLCHAIN_FILE="$arch_path/lib/cmake/Qt6/qt.toolchain.cmake" \
    -D QT_ANDROID_ABIS=$arch \
    -D QT_HOST_PATH="$Qt_HOST_DIR" \
    -D QT_PATH_ANDROID_ABI_$arch="$arch_path"
cmake -B $build_dir

cmake --build $build_dir --parallel 2

mv $build_dir/android-build/tflite-examples.apk tflite-examples_${VERSION}_$arch.apk

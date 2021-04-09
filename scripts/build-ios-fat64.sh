#!/usr/bin/env bash
#
# Copyright (c) Facebook, Inc. and its affiliates.
# All rights reserved.
#
# This source code is licensed under the BSD-style license found in the
# LICENSE file in the root directory of this source tree.

set -ex

if [ -z "$1" ]; then
	echo "Output path must be provided"
	exit 1
fi

OUT_DIR="$1"
shift 1

CMAKE_ARGS_COMMON=()

# CMake-level configuration
CMAKE_ARGS_COMMON+=("-DCMAKE_TOOLCHAIN_FILE=ios.toolchain.cmake")
CMAKE_ARGS_COMMON+=("-DCMAKE_BUILD_TYPE=Release")
CMAKE_ARGS_COMMON+=("-DCMAKE_POSITION_INDEPENDENT_CODE=ON")

# XNNPACK-specific options
CMAKE_ARGS_COMMON+=("-DXNNPACK_LIBRARY_TYPE=static")
CMAKE_ARGS_COMMON+=("-DXNNPACK_BUILD_BENCHMARKS=OFF")
CMAKE_ARGS_COMMON+=("-DXNNPACK_BUILD_TESTS=OFF")

# iOS-specific options
CMAKE_ARGS_COMMON+=("-DENABLE_BITCODE=OFF")
CMAKE_ARGS_COMMON+=("-DENABLE_ARC=ON")
CMAKE_ARGS_COMMON+=("-DDEPLOYMENT_TARGET=13.0")
CMAKE_ARGS_COMMON+=("-DENABLE_VISIBILITY=ON")

# Use-specified CMake arguments go last to allow overriding defaults
CMAKE_ARGS_COMMON+=($@)

# Build for arm64 (device)
CMAKE_ARGS_ARM=("${CMAKE_ARGS_COMMON[@]}")
CMAKE_ARGS_ARM+=("-DPLATFORM=OS64")
CMAKE_ARGS_ARM+=("-DIOS_ARCH=arm64")

ROOT_DIR=$(pwd)

INTERMEDIATE_DIR="${OUT_DIR}"/ios/intermediate
mkdir -p "${INTERMEDIATE_DIR}"

ARM_DIR="${OUT_DIR}"/ios/arm64
mkdir -p "${ARM_DIR}"

echo "[+] Building for arm64"
pushd "${ARM_DIR}"
cmake "${ROOT_DIR}" "${CMAKE_ARGS_ARM[@]}"
cmake --build . -- "-j$(sysctl -n hw.physicalcpu)"
popd

echo "[+] Merging arm64 libraries"
libtool -static -o "${INTERMEDIATE_DIR}"/libXNNPACK_arm64.a $(find ${ARM_DIR} -name "*.a")

# Build for x64 (simulator)
CMAKE_ARGS_X64=("${CMAKE_ARGS_COMMON[@]}")
CMAKE_ARGS_X64+=("-DPLATFORM=SIMULATOR64")
CMAKE_ARGS_X64+=("-DIOS_ARCH=x86_64")

X64_DIR="${OUT_DIR}"/ios/x86_64
mkdir -p "${X64_DIR}"

echo "[+] Building for x64"
pushd "${X64_DIR}"
cmake ../../.. "${CMAKE_ARGS_X64[@]}"
cmake --build . -- "-j$(sysctl -n hw.physicalcpu)"
popd

echo "[+] Merging x64 libraries"
libtool -static -o "${INTERMEDIATE_DIR}"/libXNNPACK_x86_64.a $(find ${X64_DIR} -name "*.a")

# Prepare framework
PRODUCT_DIR="${OUT_DIR}"/product
FRAMEWORK_DIR="${PRODUCT_DIR}"/XNNPACK.framework
INCLUDE_DIR="${FRAMEWORK_DIR}"/Headers
LIB_DIR="${FRAMEWORK_DIR}"

mkdir -p "${LIB_DIR}"
mkdir -p "${INCLUDE_DIR}"

echo "[+] Creating universal staic lib"
lipo "${INTERMEDIATE_DIR}"/libXNNPACK_arm64.a "${INTERMEDIATE_DIR}"/libXNNPACK_x86_64.a -create -o "${LIB_DIR}"/XNNPACK

echo "[+] Copying Headers"
cp include/xnnpack.h "${INCLUDE_DIR}"
cp "${ARM_DIR}"/pthreadpool-source/include/pthreadpool.h "${INCLUDE_DIR}"

echo "[+] Build complete. The result framework can be found at ${PRODUCT_DIR}."

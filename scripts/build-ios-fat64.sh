#!/usr/bin/env bash
#
# Copyright (c) Facebook, Inc. and its affiliates.
# All rights reserved.
#
# This source code is licensed under the BSD-style license found in the
# LICENSE file in the root directory of this source tree.

set -e

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
CMAKE_ARGS_COMMON+=("-DENABLE_ARC=OFF")
CMAKE_ARGS_COMMON+=("-DDEPLOYMENT_TARGET=12.0")
CMAKE_ARGS_COMMON+=("-DENABLE_VISIBILITY=ON")

# Use-specified CMake arguments go last to allow overridding defaults
CMAKE_ARGS_COMMON+=($@)

#Build for device

CMAKE_ARGS_ARM=("${CMAKE_ARGS_COMMON[@]}")
CMAKE_ARGS_ARM+=("-DPLATFORM=OS64")
CMAKE_ARGS_ARM+=("-DIOS_ARCH=arm64")

ARM_DIR=build/ios/arm64
mkdir -p "${ARM_DIR}"

cd "${ARM_DIR}" && cmake ../../.. "${CMAKE_ARGS_ARM[@]}" && cmake --build . -- "-j$(nproc)"
libtool -static -o libXNNPACK.a libXNNPACK.a clog/libclog.a cpuinfo/libcpuinfo.a pthreadpool/libpthreadpool.a

# Build for simulator
CMAKE_ARGS_X86=("${CMAKE_ARGS_COMMON[@]}")
CMAKE_ARGS_X86+=("-DPLATFORM=SIMULATOR64")
CMAKE_ARGS_X86+=("-DIOS_ARCH=x86_64")

cd ../../..

X86_DIR=build/ios/x86_64
mkdir -p "${X86_DIR}"

cd "${X86_DIR}" && cmake ../../.. "${CMAKE_ARGS_X86[@]}" && cmake --build . -- "-j$(nproc)"
libtool -static -o libXNNPACK.a libXNNPACK.a clog/libclog.a cpuinfo/libcpuinfo.a pthreadpool/libpthreadpool.a

# Prepare installation artefacts
INSTALL_DIR=build/install
INCLUDE_DIR="${INSTALL_DIR}"/include
LIB_DIR="${INSTALL_DIR}"/lib

cd ../../..
mkdir -p "${INCLUDE_DIR}"
mkdir -p "${LIB_DIR}"

lipo "${ARM_DIR}"/libXNNPACK.a "${X86_DIR}"/libXNNPACK.a -create -o "${LIB_DIR}"/libXNNPACK.a

cp include/xnnpack.h "${INCLUDE_DIR}"
cp "${ARM_DIR}"/pthreadpool-source/include/pthreadpool.h "${INCLUDE_DIR}"

echo "Build finished. Installation files can be found under ${INSTALL_DIR}."

#!/usr/bin/env bash
#
# Copyright (c) Facebook, Inc. and its affiliates.
# All rights reserved.
#
# This source code is licensed under the BSD-style license found in the
# LICENSE file in the root directory of this source tree.

set -e

mkdir -p build/ios/arm64

CMAKE_ARGS=()

# CMake-level configuration
CMAKE_ARGS+=("-DCMAKE_TOOLCHAIN_FILE=ios.toolchain.cmake")
CMAKE_ARGS+=("-DCMAKE_BUILD_TYPE=Release")
CMAKE_ARGS+=("-DCMAKE_POSITION_INDEPENDENT_CODE=ON")

# XNNPACK-specific options
CMAKE_ARGS+=("-DXNNPACK_LIBRARY_TYPE=static")
CMAKE_ARGS+=("-DXNNPACK_BUILD_BENCHMARKS=OFF")
CMAKE_ARGS+=("-DXNNPACK_BUILD_TESTS=OFF")

# iOS-specific options
CMAKE_ARGS+=("-DPLATFORM=OS64")
CMAKE_ARGS+=("-DIOS_ARCH=arm64")
CMAKE_ARGS+=("-DENABLE_BITCODE=OFF")
CMAKE_ARGS+=("-DENABLE_ARC=OFF")
CMAKE_ARGS+=("-DDEPLOYMENT_TARGET=12.0")
CMAKE_ARGS+=("-DENABLE_VISIBILITY=ON")

# Use-specified CMake arguments go last to allow overridding defaults
CMAKE_ARGS+=($@)

cd build/ios/arm64 && cmake ../../.. \
    "${CMAKE_ARGS[@]}"

# Cross-platform parallel build
if [ "$(uname)" == "Darwin" ]
then
  cmake --build . -- "-j$(sysctl -n hw.ncpu)"
else
  cmake --build . -- "-j$(nproc)"
fi

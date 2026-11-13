#!/bin/bash

set -e

mkdir -p $PWD/build-2026
cd $PWD/build-2026

rm -rf *

cmake \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DMITK_DIR=/data/clan2201/MITK-superbuild/MITK-build \
    -DQt6_DIR=/data/opt/Qt/6.11.2/gcc_64/lib/cmake/Qt6 \
    ..

cmake --build .
#cmake --build . --parallel
# cmake --build . -j 4

#!/bin/bash

set -e

mkdir -p $PWD/build-2025
cd $PWD/build-2025

rm -rf *

cmake \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DMITK_DIR=$HOME/MI-Brain/MITK/mitk-superbuild/MITK-build \
    -DQt6_DIR=$HOME/Qt/6.9.2/macos/lib/cmake/Qt6 \
    ..

cmake --build . --parallel
# cmake --build . -j 4

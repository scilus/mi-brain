#!/bin/bash

set -e

cd ~/mi-brain/mi-brain/build-2025

rm -rf *

cmake \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DMITK_DIR=$HOME/Libraries/build/mitk-v2025.12/MITK-build \
    -DQt6_DIR=$HOME/Libraries/install/qt-6.8.3/lib/cmake/Qt6 \
    ..

# cmake --build . --parallel
cmake --build . -j 4
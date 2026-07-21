#!/bin/bash

set -e

mkdir -p ~/mi-brain/mi-brain/build
cd ~/mi-brain/mi-brain/build

rm -rf *

cmake \
    -DSCIL_USE_SUPERBUILD=ON \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DCMAKE_BUILD_TYPE=Release \
    -DMITK_DIR=$HOME/Libraries/build/mitk-v2025.12.2/MITK-build \
    -DQt6_DIR=$HOME/Libraries/install/qt-6.8.3/lib/cmake/Qt6 \
    ..

cmake --build . --parallel

cmake --install SCIL-build --prefix "$PWD/../MI-Brain-rel"

cd ..
tar -czf MI-Brain-linux-x86_64.tar.gz MI-Brain-rel

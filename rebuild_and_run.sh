#!/bin/bash

cd ~/mi-brain/mi-brain/build-2025
rm -rf *

cmake -DCMAKE_EXPORT_COMPILE_COMMAND=ON -DMITK_DIR=$HOME/Libraries/build/mitk-2025.12-build/MITK-build -DQt6_DIR=$HOME/Libraries/install/qt-6.8.3/lib/cmake/Qt6 ..

cmake --build . --parallel

cd ..
./run_mi_brain.sh
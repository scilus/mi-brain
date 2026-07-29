#!/bin/bash

set -e

mkdir -p $PWD/build
cd $PWD/build

rm -rf *

echo "Building MI-Brain for release"

cmake \
    -DSCIL_USE_SUPERBUILD=ON \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DCMAKE_BUILD_TYPE=Release \
    -DMITK_DIR=$HOME/MI-Brain/MITK/mitk-superbuild/MITK-build \
    -DQt6_DIR=$HOME/Qt/6.9.2/macos/lib/cmake/Qt6 \
    ..

cmake --build . --parallel

cd ..
rm -rf "$PWD/MI-Brain-rel"

cmake --install "$PWD/build/SCIL-build" --prefix "$PWD/MI-Brain-rel"

# Mac specific qt deployment

APP="$PWD/MI-Brain-rel/MI-Brain.app"
QT_BIN="$HOME/Qt/6.9.2/macos/bin"
SIGN_IDENTITY="Developer ID Application: USHERBROOKE"

find "$APP/Contents/PlugIns" -type f -name "*.dylib" | while read plugin
do
    echo "Fixing $plugin"

    install_name_tool \
        -add_rpath "@loader_path/../../Frameworks" \
        "$plugin" 2>/dev/null || true
    install_name_tool \
        -delete_rpath "@loader_path/../../lib" \
        "$plugin" 2>/dev/null || true

done
        
echo "Signing app..."
codesign \
    --force \
    --deep \
    --options runtime \
    --timestamp \
    --sign "$SIGN_IDENTITY" \
    "$APP"

echo "Checking signature..."
codesign --verify --deep --strict --verbose=2 "$APP"

echo "Creating DMG..."
hdiutil create \
    -volname "MI-Brain" \
    -srcfolder "$APP" \
    -ov \
    -format UDZO \
    MI-Brain.dmg

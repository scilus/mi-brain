#!/bin/bash

set -e

ARCH="$(uname -m)"

while [[ $# -gt 0 ]]; do
        case "$1" in
                -a|--arch)
                        case "$2" in
                                "arm64"|"x86_64")
                                        ARCH="$2"
                                        shift 2 # Past argument and value
                                        ;;
                                *)
                                        echo "Error: Invalid argument '$2' for architecture. Allowed values are: arm64 or x86_64."
                                        exit 1
                                        ;;
                        esac
                        ;;
                *)
                        echo "Unknown option: $1"
                        exit 1
                        ;;
        esac
done

CURRENT_ARCH=$(uname -m)
if [[ "$CURRENT_ARCH" != "$ARCH" ]]; then
    echo "Restarting under ${ARCH} (currently ${CURRENT_ARCH})..."
    exec arch -${ARCH} "$0" "$@"
fi

mkdir -p $PWD/build
cd $PWD/build

rm -rf *

echo "Building MI-Brain for release"

cmake \
    -DSCIL_USE_SUPERBUILD=ON \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_OSX_ARCHITECTURES="$ARCH" \
    -DMITK_DIR=$HOME/MI-Brain/MITK/$ARCH/mitk-superbuild/MITK-build \
    -DQt6_DIR=$HOME/Qt/6.9.2/macos/lib/cmake/Qt6 \
    ..

cmake --build . --parallel

cd ..
rm -rf -f "$PWD/MI-Brain-$ARCH"

cmake --install "$PWD/build/SCIL-build" --prefix "$PWD/MI-Brain-$ARCH"

# Mac specific qt deployment

APP="$PWD/MI-Brain-mac-$ARCH/MI-Brain.app"
QT_BIN="$HOME/Qt/6.9.2/macos/bin"

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
#SIGN_IDENTITY="Developer ID Application: <name>"
#codesign \
#    --force \
#    --deep \
#    --options runtime \
#    --timestamp \
#    --sign "$SIGN_IDENTITY" \
#    "$APP"

codesign \
    --force \
    --deep \
    --timestamp \
    --sign - \
    "$APP"

echo "Checking signature..."
codesign --verify --deep --strict --verbose=2 "$APP"

echo "Creating DMG..."
hdiutil create \
    -volname "MI-Brain" \
    -srcfolder "$APP" \
    -ov \
    -format UDZO \
    MI-Brain-mac-$ARCH.dmg

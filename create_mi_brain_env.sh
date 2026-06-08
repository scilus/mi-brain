#!/bin/bash

MITK_PLUGINS_DIR="$HOME/Libraries/build/mitk-2025.12-build/MITK-build/lib/plugins"
MITK_BIN_DIR="$HOME/Libraries/build/mitk-2025.12-build/MITK-build/bin"
MI_BRAIN_BIN_DIR="$HOME/mi-brain/mi-brain/build-2025/bin"
QT_DIR="$HOME/Libraries/install/qt-6.8.3"

export LD_LIBRARY_PATH="${MITK_PLUGINS_DIR}:${MITK_BIN_DIR}:${MI_BRAIN_BIN_DIR}:${QT_DIR}/lib:${LD_LIBRARY_PATH}"
export QT_PLUGIN_PATH=${QT_DIR}/plugins
export QT_QPA_PLATFORM_PLUGIN_PATH=${QT_DIR}/plugins/platforms
export QT_QPA_PLATFORM=xcb

env > .vscode/mi-brain.env
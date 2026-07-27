#!/bin/bash

# Set the path to the MITK plugins directory
export MITK_PLUGINS_DIR="$HOME/Libraries/build/mitk-v2025.12.2/MITK-build/lib/plugins"
export MITK_BIN_DIR="$HOME/Libraries/build/mitk-v2025.12.2/MITK-build/bin"

# Set the path to the MI-Brain executable directory
export MI_BRAIN_BIN_DIR="$PWD/build-2025/bin"

export QT_DIR="$HOME/Libraries/install/qt-6.8.3"

# Export LD_LIBRARY_PATH to include all necessary directories
export LD_LIBRARY_PATH="${MITK_PLUGINS_DIR}:${MITK_BIN_DIR}:${MI_BRAIN_BIN_DIR}:${QT_DIR}/lib:${LD_LIBRARY_PATH}"

# Export QT_PLUGIN_PATH and QT_QPA_PLATFORM_PLUGIN_PATH to include the MITK plugins directory
export QT_PLUGIN_PATH=${QT_DIR}/plugins
export QT_QPA_PLATFORM_PLUGIN_PATH=${QT_DIR}/plugins/platforms
export QT_QPA_PLATFORM=xcb

# Execute the MI-Brain application
exec "${MI_BRAIN_BIN_DIR}/MI-Brain"

#!/bin/bash

# Set the path to the MITK plugins directory
MITK_PLUGINS_DIR="/home/local/USHERBROOKE/rhef1902/Libraries/MITK/MITK-superbuild/MITK-build/lib/plugins"
MITK_BIN_DIR="/home/local/USHERBROOKE/rhef1902/Libraries/MITK/MITK-superbuild/MITK-build/bin"

# Set the path to the MI-Brain executable directory
MI_BRAIN_BIN_DIR="/home/local/USHERBROOKE/rhef1902/Libraries/mi-brain/build-2025/bin"

# Export LD_LIBRARY_PATH to include all necessary directories
export LD_LIBRARY_PATH="${MITK_PLUGINS_DIR}:${MITK_BIN_DIR}:${MI_BRAIN_BIN_DIR}:${LD_LIBRARY_PATH}"

# Execute the MI-Brain application
exec "${MI_BRAIN_BIN_DIR}/MI-Brain"
# Architecture

## Overview
MI-Brain is built on the **MITK (Medical Imaging Interaction Toolkit)** framework, utilizing a layered and modular architecture.

## Layers
1. **Application Layer (`Apps/`)**: Initializes the `mitk::BaseApplication` and sets the main application plugin.
2. **Plugin Layer (`Plugins/`)**: Uses the **BlueBerry** (CTK-based) plugin system for UI (Views) and user interactions.
3. **Module Layer (`Modules/`)**: C++ libraries using the MITK module system for core logic.
4. **External Layer**: Managed via a SuperBuild system, integrating MITK, VTK, ITK, and Qt.

## Core Components
- **Apps** load **Plugins**.
- **Plugins** provide the interface and depend on **Modules**.
- **Modules** implement core algorithms and data structures, depending on other modules and MITK/VTK/ITK.

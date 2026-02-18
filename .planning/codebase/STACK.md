# Stack

## Languages
- **C++17:** Primary development language (required by MITK 2025.12).

## Build System
- **CMake:** Version 3.22+ required. Uses a SuperBuild pattern (`SuperBuild.cmake`) to manage dependencies.

## Core Frameworks
- **MITK (Medical Imaging Interaction Toolkit) 2025.12:** The foundational framework for the application.
- **Qt 6:** Graphical user interface framework.
- **ITK (Insight Segmentation and Registration Toolkit):** Image processing library.
- **VTK (Visualization Toolkit):** 3D rendering and visualization library.

## Key Dependencies
- **CTK (Common Toolkit) / BlueBerry:** Plugin architecture and workbench UI.
- **OpenMP:** Multi-threaded parallel processing.
- **DCMTK / GDCM:** DICOM support.
- **Boost:** Utility library.
- **OpenSSL:** Secure communications (referenced in Windows builds).

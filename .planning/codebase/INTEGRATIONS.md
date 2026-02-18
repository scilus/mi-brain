# Integrations

## Medical Imaging Formats
- **DICOM:** Supported via MITK and internal `DicomWriter.cpp` in `Modules/ImekaIO/`.
- **Diffusion/Fiber Formats:** Support for `.tck` (Tractography), `.trk` (TrackVis), and `.vtk` files (found in `Modules/DiffusionIO/`).
- **Custom Imeka Formats:** FDF (`FDFReader.hpp`) and VFF (`VFFReader.hpp`) loaders.

## Internal Module Structure
- `Modules/FiberBundle`: Core fiber-tracking logic.
- `Modules/ImekaIO`: Primary I/O handling for custom and standard formats.
- `Modules/ImekaCommon`: Shared utilities and data management.

## External Services
- **Local Application:** Standalone desktop medical imaging tool. No cloud or web API integrations detected.

## Authentication
- **None:** Uses local filesystem access.

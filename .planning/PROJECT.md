# MI-Brain MITK 2025 Port

## What This Is
Porting the MI-Brain medical imaging application from a legacy MITK dev2018 codebase to MITK 2025. This project focuses on resolving core rendering anomalies in streamline visualization and fixing coordinate synchronization issues introduced by the framework migration.

## Core Value
High-fidelity, cross-platform tractography visualization that is spatially accurate and visually consistent across 2D sliced views and 3D representations.

## Requirements

### Validated
- ✓ **Core Architecture:** Built on MITK 2025.12 framework using CMake SuperBuild. — existing
- ✓ **UI Framework:** Qt 6 migration is largely functional. — existing
- ✓ **Tractography (TCK):** TCK files are correctly parsed and rendered in both 2D and 3D views. — existing
- ✓ **Cross-Platform Support:** Compatible with Linux and Windows. — existing

### Active
- [ ] **Fix Streamline Slicing (2D):** Resolve the "stitching" bug where independent streamlines are incorrectly connected end-to-end in 2D views.
- [ ] **Fix TRK Geometry Sync:** Correct the spatial misalignment of TRK files in 2D slices while preserving 3D accuracy (RAS/LPS synchronization).
- [ ] **Fix Fiber Shaders:** Address shader-level rendering issues where streamlines are incorrectly linked or incorrectly thickened.
- [ ] **Update FiberBundleMapper2D:** Refactor the mapper to use modern VTK 9.4+ slicing logic (vtkCutter/vtkPlane) and MTime-based updates.
- [ ] **Verify Visual Elements:** Manual verification of streamline fidelity and 2D/3D synchronization across critical views.

### Out of Scope
- **New Features:** No new analysis tools or UI panels are being added during this port.
- **Old MITK Compatibility:** We are not maintaining backward compatibility with dev2018.

## Constraints
- **Framework:** Must use MITK 2025 (and its dependencies: VTK 9.4+, ITK 5.x, Qt 6).
- **Architecture:** Must adhere to MI-Brain's existing Module/Plugin/App structure.
- **Hardware:** Must run on standard cross-platform OpenGL-capable systems.

## Key Decisions

| Decision | Rationale | Outcome |
|----------|-----------|---------|
| Manual Verification | Visual fidelity is the primary success metric for clinicians/users. | Pending |
| TCK as Reference | TCK files are correctly handled, providing a ground truth for TRK fixes. | Pending |
| Refactor 2D Mapper | The legacy shader-based discard logic is incompatible with modern VTK. | Pending |

---
*Last updated: February 18, 2026 after initialization*

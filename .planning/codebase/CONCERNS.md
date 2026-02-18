# Codebase Concerns

**Analysis Date:** 2025-02-27

## Tech Debt

**MITK 2025 Migration:**
- Issue: Several critical readers and writers are commented out and disabled pending migration to newer APIs (TinyXML2 and DCMTK).
- Files: `Modules/DiffusionIO/mitkDiffusionModuleActivator.cpp`
- Impact: Functionality for reading/writing certain fiber bundle formats (VTK, Dicom) is currently disabled.
- Fix approach: Complete the migration to TinyXML2 and update DCMTK API usage as noted in the TODOs.

**VTK Shader API Migration:**
- Issue: Custom shaders need to be migrated to the new VTK shader replacement API.
- Files: 
  - `Modules/DiffusionIO/mitkFiberBundleMapper2D.cpp`
  - `Modules/ImekaFiber/ImekaFiber/Mapper/MitkFiberMapper2D.cpp`
- Impact: Rendering may fail or be deprecated in newer VTK versions; potential visual artifacts.
- Fix approach: Refactor shader implementations to use the new `AddShaderReplacement` API.

**Workbench Window Hack:**
- Issue: Direct usage of Berry classes outside of MITK code to manipulate the Help menu. Explicitly marked as "BAD" and "A HACK".
- Files: `Plugins/org.imeka.common.app/src/CommonWorkbenchWindowAdvisor.cpp`
- Impact: High fragility; likely to break with MITK/Berry updates.
- Fix approach: Investigate proper extension points (`org.blueberry.ui.help.aboutAction`) or alternative menu contribution mechanisms provided by the framework.

## Known Bugs

**Incorrect Maxima Logic:**
- Symptoms: Logic for maximums is hardcoded to 3, which the comment admits is "not right".
- Files: `Modules/ImekaFiber/ImekaFiber/Glyph/Maxima.cpp`
- Trigger: Glyph generation or visualization involving maxima.
- Workaround: None documented.

**Unused/Legacy Code:**
- Symptoms: Presence of code flagged with "Do we really need (or even use) this?".
- Files: `Modules/ImekaFiber/ImekaFiber/utils.cpp`
- Trigger: N/A
- Workaround: Clean up unused functions.

## Performance Bottlenecks

**Fiber Bundle Processing:**
- Problem: Large loops iterating over fiber points and cells. While some OpenMP parallelization exists, `critical` sections are heavily used, potentially creating serialization bottlenecks.
- Files: `Modules/FiberBundle/FiberBundle/mitkFiberBundle.cpp`
- Cause: Iterating over `m_FiberPolyData` cells and points for processing (smoothing, coloring).
- Improvement path: Optimize OpenMP usage to minimize `critical` sections; investigate vectorization or GPU acceleration for heavy geometric operations.

**Large Monolithic Files:**
- Problem: Several files are very large (>500 lines), indicating mixed responsibilities and complexity.
- Files: 
  - `Modules/FiberBundle/FiberBundle/mitkFiberBundle.cpp` (967 lines)
  - `Modules/ImekaFiber/ImekaFiber/Filtering/Filtering.cpp` (774 lines)
- Cause: Accumulation of features over time.
- Improvement path: Refactor into smaller, focused service classes or helper utilities.

## Fragile Areas

**DCMTK/TinyXML2 Dependencies:**
- Files: `Modules/DiffusionIO/mitkDiffusionModuleActivator.cpp`
- Why fragile: The code is currently in a transition state with functionality explicitly commented out. Any changes here before migration is complete risk further breakage.
- Safe modification: Prioritize enabling the disabled readers/writers before adding new features.
- Test coverage: Disabled code cannot be tested.

## Missing Critical Features

**Fiber I/O Formats:**
- Problem: VTK and DICOM fiber bundle readers/writers are disabled.
- Blocks: Loading/saving these standard formats.

---

*Concerns audit: 2025-02-27*

# TRX implementation in MI-Brain

This document explains how support for the [TRX tractography format](https://github.com/tee-ar-ex/trx-spec)
(`.trx`) was added to MI-Brain: how files are read and written, which pieces of
code were added, how the `trx-cpp` library is linked, and which existing
MITK fiber bundle readers/writers served as templates.

Branch: `clan2201-TrxImplementation`.

---

## 1. Overview

TRX support follows the exact same structure as the existing `.tck` support:

```
                    MITK IO framework (mitk::IOUtil, File > Open / Save)
                                       │
                 mime type "…FiberBundle.trx" (extension .trx)
                                       │
          ┌────────────────────────────┴────────────────────────────┐
          │ Modules/DiffusionIO  (MITK plumbing, no trx-cpp include) │
          │   mitkFiberBundleTrxReader   mitkFiberBundleTrxWriter    │
          └────────────────────────────┬────────────────────────────┘
                                       │ ReadTrx() / WriteTrx()
          ┌────────────────────────────┴────────────────────────────┐
          │ Modules/FiberBundle  (format logic, links trx-cpp)       │
          │   FiberBundle/Trx.h, FiberBundle/Trx.cpp                 │
          └────────────────────────────┬────────────────────────────┘
                                       │ trx::load_any / trx::TrxStream
                                  trx-cpp (libtrx)
```

- **DiffusionIO** holds thin MITK `AbstractFileReader` / `AbstractFileWriter`
  classes. They are registered as micro-services, handle the locale, file names
  and error reporting, and delegate the actual work.
- **FiberBundle** holds the format conversion (`ReadTrx` / `WriteTrx`) between
  a TRX file and a `vtkPolyData` stored in a `mitk::FilteredFiberBundle`. This is
  the only place that includes `<trx/trx.h>`.

This split mirrors `Tck.hpp/Tck.cpp` (in FiberBundle) +
`mitkFiberBundleTckReader/Writer` (in DiffusionIO).

---

## 2. How reading works

Entry point: `mitk::FiberBundleTrxReader::DoRead()`
(`Modules/DiffusionIO/mitkFiberBundleTrxReader.cpp`).

1. A `LocaleGuard` (RAII) switches `LC_ALL` to `"C"` and restores the previous
   locale on scope exit, including when an exception is thrown.
2. A `mitk::FilteredFiberBundle` is created and passed to `ReadTrx(filename, fiber)`.
3. If `ReadTrx` returns 0 points, a `std::length_error` is thrown (same message
   and behaviour as the TCK reader), so empty files never reach the DataManager.
4. The fiber bundle is returned to MITK.

`ReadTrx()` (`Modules/FiberBundle/FiberBundle/Trx.cpp`):

1. **Load** – `trx::load_any(filename)` opens the file whatever its position
   dtype (float16/32/64), and whether it is a zipped `.trx` or a directory. It
   returns a `trx::AnyTrxFile` whose arrays are memory-mapped.
2. **Positions → `vtkPoints`** – `TrxToPolyData()` allocates one
   `vtkFloatArray` of `num_vertices()` × 3, then walks the positions with
   `for_each_positions_chunk()` in 64 MB chunks. Each chunk is converted by
   `ConvertPositions<T>()` (`T` = `Eigen::half`, `float` or `double`):
   - values are copied with `memcpy`, because positions inside an uncompressed
     zip are not guaranteed to be aligned;
   - **coordinates are converted from RAS+ mm (TRX) to LPS mm (MITK world)** by
     negating X and Y – the same `(-x, -y, z)` flip done by `ReadTck`.
3. **Offsets → `vtkCellArray`** – TRX stores all streamlines contiguously with an
   `offsets` array giving the first point of each streamline. This is exactly
   VTK's modern cell-array layout (offsets + connectivity), so:
   - TRX `offsets` are copied into a `vtkIdTypeArray`; a trailing sentinel equal
     to the number of points is appended if the file omits it;
   - connectivity is simply `0, 1, 2, … N-1`;
   - `vtkCellArray::SetData(offsets, connectivity)` builds all lines at once,
     without creating one `vtkPolyLine` per streamline as `ReadTck` does.
4. **Groups** – `ReadGroups()` copies `groups/*` into a
   `FilteredFiberBundle::StreamlineGroups` (name -> streamline indices), before
   the file is closed (see §8).
5. **Hand-off** – `trxFile.close()` releases the mapping (and the temporary
   directory trx-cpp extracts zipped files into), then
   `fiber->SetFiberPolyData(polyData)` stores the data (it deep-copies, so
   closing first is safe) and `fiber->SetStreamlineGroups(groups)` stores the
   groups.

## 3. How writing works

Entry point: `mitk::FiberBundleTrxWriter::Write()`
(`Modules/DiffusionIO/mitkFiberBundleTrxWriter.cpp`).

1. The input is cast to `mitk::FiberBundle`; anything else raises `mitkThrow()`.
2. If the output location has no extension, `.trx` is appended. This must be done
   **before** creating the `LocalFile`, because the temporary file name is built
   from it and trx-cpp chooses the output format from the extension.
3. `LocaleGuard` sets the `"C"` locale.
4. `mitk::AbstractFileWriter::LocalFile` provides a real file path: the output
   location, or a temporary file copied into the output stream when MITK writes
   to a stream (trx-cpp can only write to files). This is a difference from the
   TCK/TrackVis writers, which open an `std::ofstream` themselves.
5. `WriteTrx(localFile.GetFileName(), input->GetFiberPolyData(), groups)` is
   called, where `groups` is `FilteredFiberBundle::GetStreamlineGroups()` (the
   input is cast to `FilteredFiberBundle`; a plain `FiberBundle` is written
   without groups). `std::exception`s from trx-cpp are turned into
   `mitkThrow()` with the message.

`WriteTrx()` (`Modules/FiberBundle/FiberBundle/Trx.cpp`):

1. Refuses an empty polydata (no points or no lines).
2. Creates a `trx::TrxStream("float32")`. The stream spools positions to a
   temporary file, so a second full copy of the bundle is never held in memory.
3. Sets the mandatory header fields `VOXEL_TO_RASMM` (identity) and
   `DIMENSIONS` (`{1, 1, 1}`), the same defaults as `trx::TrxFile`, because no
   reference image is known at this point.
4. Iterates the lines with `vtkCellArray::GetNextCell`, converts each point from
   LPS to RAS (`(-x, -y, z)`), and calls `push_streamline()` with a reused
   buffer.
5. `WriteGroups()` writes the streamline groups passed by the writer with
   `push_group_from_indices()`, as `uint32` groups. Each group's indices are
   checked against the number of streamlines, sorted and deduplicated; empty
   groups are skipped; names are sanitized by `TrxGroupName()` and made
   unique with a `_2`, `_3`, ... suffix if two become identical (see §8).
6. `stream.finalize<float>(filename)` writes the final `.trx` file.

Not handled yet: data per point / per streamline / per group (`dpp`, `dps`,
`dpg`), and a
real reference geometry (`VOXEL_TO_RASMM`, `DIMENSIONS`) on read or write.

---

## 4. Files added or modified

### New files

| File | Role |
|---|---|
| `Modules/FiberBundle/FiberBundle/Trx.h` | Declares `ReadTrx()` and `WriteTrx()` (exported with `FiberBundle_EXPORT`). Only VTK/std types in the interface, so trx-cpp stays private. |
| `Modules/FiberBundle/FiberBundle/Trx.cpp` | TRX ⇄ `vtkPolyData` conversion (see §2 and §3). |
| `Modules/DiffusionIO/mitkFiberBundleTrxReader.h/.cpp` | `mitk::FiberBundleTrxReader`, an `AbstractFileReader` registered for the TRX mime type. |
| `Modules/DiffusionIO/mitkFiberBundleTrxWriter.h/.cpp` | `mitk::FiberBundleTrxWriter`, an `AbstractFileWriter` for `FilteredFiberBundle` and the TRX mime type. Passes the bundle's streamline groups to `WriteTrx()`. |
| `Modules/ImekaFiber/Testing/TrxGroupsTest.cpp` | `TrxGroupsTest` suite: group nodes, visibility, saving, TRX round trip, shuffle and saver regressions (see §8). |

### Modified files

| File | Change |
|---|---|
| `Modules/DiffusionIO/mitkDiffusionIOMimeTypes.h/.cpp` | New `FIBERBUNDLE_TRX_MIMETYPE()` (extension `trx`, category/comment "Trx Fibers") and `FIBERBUNDLE_TRX_MIMETYPE_NAME()` (`<base>.FiberBundle.trx`); added to `DiffusionIOMimeTypes::Get()`. |
| `Modules/DiffusionIO/mitkDiffusionModuleActivator.cpp` | Includes the new headers, gives the TRX mime type a service ranking of `-5`, and creates/deletes `m_FiberBundleTrxReader` and `m_FiberBundleTrxWriter` in `Load()`/`Unload()`. |
| `Modules/DiffusionIO/files.cmake` | Adds `mitkFiberBundleTrxReader.cpp` and `mitkFiberBundleTrxWriter.cpp`. |
| `Modules/FiberBundle/files.cmake` | Adds `FiberBundle/Trx.cpp`. |
| `Modules/FiberBundle/FiberBundle/FilteredFiberBundle.cpp` | Adds `"Fibers TRX (*.trx)"` to `Filters()`, so the "Save visible fibers to file(s)..." dialogs offer TRX (the save goes through `mitk::IOUtil::Save`, which picks the TRX writer from the extension). Colors are not saved in TRX. |
| `Modules/FiberBundle/CMakeLists.txt` | Links `trx-cpp::trx` privately (see §5). |
| `CMakeLists.txt` (root) | New "TRX" section that finds Eigen and builds trx-cpp (see §5). |
| `Modules/DiffusionIO/CMakeLists.txt` | Only formatting + a commented note about Eigen; the module does **not** link trx-cpp. |
| `build_mi_brain.sh` | Local paths (`build-2026`, MITK and Qt6 dirs for this machine), non-parallel build. |
| `Modules/FiberBundle/FiberBundle/FilteredFiberBundle.hpp/.cpp` | `StreamlineGroups` type with `SetStreamlineGroups()` / `GetStreamlineGroups()`. `Shuffle()` now returns the new order and remaps the groups. |
| `Modules/ImekaFiber/ImekaFiber/types.hpp` | `TractGroup::fixedIndexes` / `IsFixed()` for the tract groups made of a fixed set of streamlines (TRX groups). |
| `Modules/ImekaFiber/ImekaFiber/Filtering/Filtering.hpp/.cpp` | `AddFixedTractGroup()`, `HasFixedTractGroup()`, `HasVisibleFixedTractGroup()`, `RemapFixedTractGroups()`; fixed TGs handled in `EnableTractGroup()`, `DisableTractGroup()`, `HasAnyActiveROI()` and `JoinAndExclude()`. |
| `Modules/ImekaFiber/ImekaFiber/Filtering/FilteringUI.hpp/.cpp` | `AddStreamlineGroups()` creates the group nodes; `HideFixedTractGroups()` resets them when a dataset or the Tracts category is shown. |
| `Modules/ImekaFiber/ImekaFiber/FibersManager.cpp` | Calls `AddStreamlineGroups()` in `FibersAdded()`; `ShuffleFibers()` remaps the fixed TGs and recomputes the length subset. |
| `Modules/ImekaFiber/ImekaFiber/Mapper/MapperData.hpp/.cpp` | `FiberBundleFromData()` records the new index of each copied streamline (`newIds`) and fills the groups of the new bundle; `AddStreamlineGroup()` helper. |
| `Modules/ImekaFiber/ImekaFiber/Saver.cpp` | The save visible fibers actions fill the groups of the saved bundle; with one visible dataset, that dataset is saved (it used to be the first one of the map). |
| `Modules/ImekaFiber/Testing/CMakeLists.txt`, `files.cmake` | Register `TrxGroupsTest`. |

---

## 5. How trx-cpp is linked to MI-Brain

### Current setup (what is in the branch)

trx-cpp is built **as part of the mi-brain CMake project** with
`add_subdirectory`, from a local clone. Root `CMakeLists.txt`, right after
`add_subdirectory(Modules)`:

```cmake
set(Eigen3_DIR "/data/clan2201/MITK-superbuild/ep/lib/cmake/ITK-5.4/Modules")
find_package(Eigen3 REQUIRED CONFIG)
set(TRX_EIGEN3_TARGET Eigen3::Eigen)
...
add_subdirectory(/data/clan2201/trx-cpp ${CMAKE_BINARY_DIR}/trx-cpp-build)
```

Key points:

- **Source**: `/data/clan2201/trx-cpp`, fork `nicolasCChaput/trx-cpp`, commit
  `76f6133` ("Merge pull request #49 from frheault/fixes_for_benchmark").
- **Eigen**: trx-cpp needs Eigen 3.4. ITK, already built by the MITK superbuild,
  ships its own Eigen (3.4.90) with an `Eigen3Config.cmake` under
  `ep/lib/cmake/ITK-5.4/Modules`. We `find_package` it and set
  `TRX_EIGEN3_TARGET=Eigen3::Eigen` *before* `add_subdirectory`. trx-cpp's
  `CMakeLists.txt` checks `if(TRX_EIGEN3_TARGET AND TARGET ${TRX_EIGEN3_TARGET})`
  and then skips its own lookup/download. Result: a single Eigen for ITK, MITK
  and trx-cpp, no version or ODR conflict.
- **libzip**: not provided by us; trx-cpp's `find_package(libzip CONFIG)` finds the
  system package (`libzip-dev`, `/usr/lib/x86_64-linux-gnu/cmake/libzip`).
  `libtrx.so` depends on `/lib/x86_64-linux-gnu/libzip.so.4` at runtime.
- **Library type**: because MITK sets `BUILD_SHARED_LIBS=ON`, trx-cpp is built
  as a shared library: `build-2026/bin/libtrx.so` (plus `libtrx-nifti.so`, and
  the examples, since `TRX_BUILD_EXAMPLES` / `TRX_ENABLE_NIFTI` default to ON).
- **Consumer**: only the `FiberBundle` module links it:

  ```cmake
  target_link_libraries(${MODULE_TARGET} PRIVATE trx-cpp::trx)
  ```

  `trx-cpp::trx` is an `ALIAS` target created by trx-cpp's CMake. Even though
  `add_subdirectory(trx-cpp)` appears after `add_subdirectory(Modules)`, CMake
  resolves link targets at generation time, so it works. The link is `PRIVATE`
  because `<trx/trx.h>` is only included from `Trx.cpp`; `Trx.h` exposes
  nothing from trx-cpp. DiffusionIO calls `ReadTrx`/`WriteTrx` through the
  exported FiberBundle functions and does not need trx-cpp at all.

Limitations: absolute paths to `/data/clan2201/...`, a hard-coded `ITK-5.4`
directory, a system libzip that would need to be shipped by the installers, and
trx-cpp's examples/nifti library built for nothing (they could be turned off
with `set(TRX_BUILD_EXAMPLES OFF)` / `set(TRX_ENABLE_NIFTI OFF)` before
`add_subdirectory`).

### Planned setup

`PLAN.md` describes the longer-term approach: build trx-cpp as a regular MITK
third-party project through `MITK_EXTENSION_DIRS`
(`third_party/mitk-extension/CMakeExternals/trx-cpp.cmake`, static + PIC, using
ITK's Eigen), and consume it in mi-brain through `PACKAGE_DEPENDS PRIVATE trx-cpp`
with a `CMake/PackageDepends/MITK_trx-cpp_Config.cmake` file. This removes the
absolute paths and makes the build reproducible on other machines/CI. It is
**not implemented yet**.

---

## 6. MITK fiber bundle code used as reference

| Reference | What was reused for TRX |
|---|---|
| `FiberBundle/Tck.hpp`, `FiberBundle/Tck.cpp` (`ReadTck`, `WriteTck`) | Main template for `Trx.h/.cpp`: same free-function API (`ReadX(filename, FilteredFiberBundle*)` returning a point count, `WriteX(filename, vtkPolyData*)`), same forward declarations and `FiberBundle_EXPORT`, same RAS ⇄ LPS flip `(-x, -y, z)`, same `fiber->SetFiberPolyData(polyData)` hand-off, same `lines->InitTraversal()` / `GetNextCell()` loop with a reused point buffer when writing. |
| `mitkFiberBundleTckReader.hpp/.cpp` | Template for `mitkFiberBundleTrxReader`: constructor with mime type name + description and `RegisterService()`, copy constructor/`Clone()`, `DoRead()` structure, the "0 point in this file" `std::length_error`, the `"C"` locale switch (made exception-safe here with `LocaleGuard`). |
| `mitkFiberBundleTckWriter.hpp/.cpp` | Template for `mitkFiberBundleTrxWriter`: `AbstractFileWriter(FilteredFiberBundle::GetStaticNameOfClass(), <mime name>, <description>)`, `Clone()`, `using AbstractFileWriter::Write`, adding the default extension when missing, casting the input to `mitk::FiberBundle` and passing `GetFiberPolyData()`. |
| `mitkFiberBundleTrackVisReader/Writer` | Second reference for the reader/writer pattern, locale handling and `mitkThrow()` error reporting. |
| `mitkFiberBundleVtkWriter` | Reference for writing through the extension check and for writer error handling. |
| `mitkDiffusionIOMimeTypes` (`FIBERBUNDLE_TCK_MIMETYPE`, `FIBERBUNDLE_TRK_MIMETYPE`) | Pattern for the TRX `CustomMimeType` and its `_NAME()` function. |
| `mitkDiffusionModuleActivator` | Existing registration of the VTK/TCK/TrackVis/DICOM readers and writers; TRX was added the same way, with its own service ranking. |
| `mitk::FilteredFiberBundle` / `mitk::FiberBundle` | Data object filled by the reader (`SetFiberPolyData`) and read by the writer (`GetFiberPolyData`). |
| `mitk::AbstractFileWriter::LocalFile` (MITK core) | Used by the TRX writer (not by the TCK one) so that trx-cpp always gets a real file path. |

---

## 7. Known issues / next steps

- **Service ranking `-5`**: fine while the TRX reader is the only one for `.trx`,
  but it would lose against any other reader of the same extension.
- **Reference geometry**: `VOXEL_TO_RASMM`/`DIMENSIONS` are ignored on read and
  set to identity/`{1,1,1}` on write. Writing a bundle loaded from `.trk` or
  `.trx` loses its original reference space.
- **Groups**: imported and exported (see §8). Not saved in scenes.
- **Shuffle with ROIs (not verified, not TRX specific)**: ROI results are only
  recomputed when the ROI changes (`Result::IsUpToDate` compares the ROI
  `MTime`), not when `Shuffle()` reorders the streamlines, so ROI selections
  may be stale after a shuffle.
- **dpp / dps / dpg**: not imported or exported.
- **Build portability**: see §5 and `PLAN.md` (superbuild integration, libzip as
  an external project, Eigen path not tied to `ITK-5.4`).
- **Validation**: compare point coordinates of the same tractogram loaded as
  `.trk`, `.tck` and `.trx` (e.g. converted with scilpy
  `scil_tractogram_convert.py`), and do a `.trx` → MI-Brain → `.trx` round trip.

---

## 8. TRX groups

- `ReadTrx()` reads `groups/*` (any integer dtype, memcpy'd for alignment,
  out-of-range indices dropped with a warning) into
  `FilteredFiberBundle::SetStreamlineGroups()` (name -> streamline indices).
- When the bundle is added to the DataStorage, `FibersManager::FibersAdded()`
  calls `FilteringUI::AddStreamlineGroups()`, which creates one **tract group**
  node per TRX group under the dataset node (Data Manager tree).
- Those are regular MI-Brain tract groups with a fixed set of streamlines
  (`TractGroup::fixedIndexes`). In `Filtering::JoinAndExclude()` the group's
  streamlines act as the first AND, so ROIs added to it still filter it.
  Group nodes are **hidden on load**, so the whole tractogram is displayed.
  A fixed tract group is active when its node is visible (the node visibility
  is the only state: hiding/showing the dataset changes its children's
  visibility without calling their callbacks). The dataset then shows the
  union of the visible tract groups; hiding them all shows everything again.
  Showing a dataset that has TRX groups (or the Tracts category above it)
  puts all its TRX group nodes back to hidden (`FilteringUI::
  HideFixedTractGroups`) and refilters it, so the whole tractogram is shown.

### Writing groups

The "Save visible fibers" actions (`Imeka::Fiber::Saver`) build a new
`FilteredFiberBundle` and now also fill its streamline groups, with indices in
the new bundle. `FiberMapperData::FiberBundleFromData` records, while copying,
the new index of each written streamline (`PartMapperData::FiberBundleFromData`,
`newIds`), then maps the groups through it:

- **Nothing selected** (no active tract group, the whole dataset is shown): the
  groups stored in the bundle (the TRX groups of the loaded file) are kept,
  restricted to the saved streamlines.
- **Some tract groups active**: one group per enabled *and* visible tract
  group, named after its node. This covers the TRX groups the user showed (the
  hidden ones are not saved) and the tract groups created by the user with ROIs
  ("New Tract Group"). A streamline in several groups is written once and
  belongs to each of them.
- **Saving a single tract group** (its Save icon): one group, named after it,
  with all the saved streamlines.
- **Save all tracts** (Tracts category): the groups of every visible dataset;
  identical names are made unique (`"AF (2)"`), never merged.

Empty groups are dropped, and duplicate names in one dataset get the same
`" (2)"` suffix (`AddStreamlineGroup`). `WriteTrx` then replaces the characters
TRX can't store in a group file name (`.`, `/`, `\`, `:`, ... → `_`, because
`groups/<name>.uint32` with a dot in `<name>` can't be read back) and logs the
renaming. Other formats (`.tck`, `.trk`, `.vtk`) ignore the groups.

Saving to the Data Manager ("dm") gives a bundle with groups too, so
`AddStreamlineGroups` shows them as tract groups under the new dataset.

When several datasets are loaded and only one is visible, the saver uses
that dataset. It used to take the first dataset of its map, which could be a
hidden one, and save its streamlines (and now its groups) instead.

### Shuffle

Groups are lists of streamline indices, and `Shuffle()` reorders the
streamlines, so both copies of the indices are remapped:

- `FilteredFiberBundle::Shuffle()` returns the new order (new streamline `i`
  is the old streamline `ordering[i]`) and remaps `m_StreamlineGroups` through
  its inverse;
- `FibersManager::ShuffleFibers()` passes that order to
  `Filtering::RemapFixedTractGroups()`, which replaces the `fixedIndexes` of
  each fixed tract group.

`ShuffleFibers()` also recalculates the length subset
(`Filtering::RecalculateSubset`) in every case, since it holds streamline
indices too (it used to be done only when no filtering was active). A visible
TRX group counts as filtering (`HasVisibleFixedTractGroup`), so the dataset
keeps showing only that group after a shuffle instead of the whole
tractogram.

### Tests

`Modules/ImekaFiber/Testing/TrxGroupsTest.cpp` (`ctest -R TrxGroups`):

| Test | Checks |
|---|---|
| `TestGroupsAreChildrenOfDataset` | One hidden tract group node per group, under the dataset; whole tractogram shown. |
| `TestHidingGroups` | Showing groups displays their union; hiding them all shows everything. |
| `TestHidingDataset` | Hiding/showing the dataset resets the groups to hidden. |
| `TestReadTrxGroups` | Groups of a real file, if `MIBRAIN_TRX_GROUPS_TEST_FILE=<file.trx>` is set. |
| `TestSaveWholeKeepsGroups` | Nothing selected: all groups are saved. |
| `TestSaveVisibleGroups` | Only the visible groups are saved; shared streamlines written once. |
| `TestSaveNewTractGroup` | A user tract group (ROI) is saved as a group named after it. |
| `TestSaveTractGroup` | Saving one tract group gives one group. |
| `TestWriteReadTrxGroups` | `.trx` round trip through the writer; `left.AF` read back as `left_AF`. |
| `TestShuffleKeepsGroups` | After shuffles, stored, displayed and saved groups are the same streamlines. |
| `TestSaveOnlyVisibleDataset` | With two datasets, the visible one is saved. |

### Known limits

- Groups are not saved in scenes.
- `dpg` (e.g. group colors) is neither read nor written.

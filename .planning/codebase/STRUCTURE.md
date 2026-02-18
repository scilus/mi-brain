# Structure

## Directory Map
- `Apps/MI-Brain/`: Main executable source and entry point.
- `Modules/`: Shared libraries containing core logic.
  - `FiberBundle`: Tractography data structures.
  - `ImekaCommon`: Utilities and persistence.
  - `ImekaIO`: File format readers and writers.
- `Plugins/`: OSGi-like bundles for UI features.
  - `org.imeka.mibrain.app`: Application lifecycle.
  - `org.imeka.mibrain.view`: Primary user interface views.
- `CMake/`: Build system macros.
- `CMakeExternals/`: External dependency management logic.
- `Testing/`: Test data and global test configuration.

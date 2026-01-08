# DiffusionIO Module

## Overview
DiffusionIO is a MITK module responsible for **Input/Output operations and visualization** of diffusion MRI tractography data. It provides comprehensive support for reading, writing, serializing, and rendering fiber bundles and peak images.

## Primary Purpose
- File I/O for multiple tractography formats
- 2D and 3D visualization of fiber bundles and diffusion peaks
- Scene serialization for fiber data
- Integration with MITK's rendering pipeline

## Supported File Formats

### 1. TrackVis Format (.trk)
- **Priority Rank**: -1 (second highest)
- **Reader**: `mitkFiberBundleTrackVisReader`
- **Writer**: `mitkFiberBundleTrackVisWriter`
- **Description**: Standard tractography format with voxel-to-RAS coordinate transform
- **Features**: Preserves fiber geometry and reference space information

### 2. MRtrix TCK Format (.tck)
- **Priority Rank**: -2
- **Reader**: `mitkFiberBundleTckReader`
- **Writer**: `mitkFiberBundleTckWriter`
- **Description**: MRtrix native tractography format
- **Features**: Efficient storage, widely used in diffusion MRI analysis

### 3. VTK PolyData Format (.vtk, .fib)
- **Priority Rank**: -3
- **Reader**: `mitkFiberBundleVtkReader`
- **Writer**: `mitkFiberBundleVtkWriter`
- **Description**: Standard VTK polydata format
- **Features**: Flexible, can store additional fiber properties

### 4. DICOM Tractography
- **Priority Rank**: -4 (lowest)
- **Reader**: `mitkFiberBundleDicomReader`
- **Writer**: `mitkFiberBundleDicomWriter`
- **Description**: DICOM standard tractography format
- **Features**: Uses DCMTK library for DICOM compliance

## Visualization Components

### FiberBundle Mappers

#### FiberBundleMapper3D
**Purpose**: Renders fiber bundles in 3D views

**Rendering Modes**:
- **Line Mode**: Basic streamline rendering (fast)
- **Tube Mode**: Volumetric tubes with adjustable radius and sides
- **Ribbon Mode**: Flat ribbon representation with adjustable width

**Features**:
- Color mapping via lookup tables
- Scalar-based coloring (orientation, curvature, length, etc.)
- Clipping plane support
- Depth sorting for transparency
- Shader-based rendering optimizations

#### FiberBundleMapper2D
**Purpose**: Renders fiber cross-sections in 2D slice views

**Features**:
- Slice plane intersection visualization
- Color consistency with 3D view
- Per-slice rendering updates

### PeakImage Mappers

#### PeakImageMapper3D
**Purpose**: Renders diffusion orientation distribution peaks in 3D

**Features**:
- Glyph-based visualization
- Direction-colored peaks
- Support for multi-peak voxels

#### PeakImageMapper2D
**Purpose**: Renders diffusion peaks in 2D slice views

**Features**:
- Slice-specific peak visualization
- Direction coloring
- Adjustable peak scaling

## Core Components

### mitkFiberTrackingObjectFactory
**Purpose**: Factory pattern for creating appropriate mappers

**Responsibilities**:
- Creates 2D/3D mappers based on data type
- Handles both `FiberBundle` and `FilteredFiberBundle`
- Sets default visualization properties
- Manages mapper lifecycle

**Supported Data Types**:
- `FiberBundle`: Basic fiber bundle
- `FilteredFiberBundle`: Fiber bundle with filtering capabilities
- `PeakImage`: Diffusion orientation peaks

### mitkDiffusionModuleActivator
**Purpose**: Module activation and service registration

**Responsibilities**:
- Auto-loads with MitkCore
- Registers all MIME types with priority ranking
- Instantiates readers/writers on module load
- Manages I/O service lifecycle
- Cleanup on module unload

### mitkDiffusionIOMimeTypes
**Purpose**: MIME type definitions and file associations

**Responsibilities**:
- Defines MIME types for all supported formats
- Provides file extension associations (.trk, .tck, .vtk, .fib)
- Enables automatic format detection
- Priority-based format selection

## Serialization

### FiberBundleSerializer
**Purpose**: Serializes FiberBundle objects for MITK scene files

**Features**:
- Preserves fiber geometry
- Maintains color information
- Compatible with MITK scene format

### FilteredFiberBundleSerializer
**Purpose**: Serializes FilteredFiberBundle objects with filtering state

**Features**:
- Preserves filtering configuration
- Maintains visibility states
- Saves color coding settings

## Dependencies

- **FiberBundle**: Core fiber data structures and algorithms
- **MitkDICOMReader**: DICOM file reading capabilities
- **MitkSceneSerializationBase**: Scene persistence framework
- **DCMTK**: DICOM toolkit for tractography I/O
- **VTK**: Visualization pipeline (polydata, mappers, filters)

## Architecture Patterns

1. **Reader/Writer Pairs**: Symmetric I/O for each format
2. **Factory Pattern**: Dynamic mapper creation
3. **Module Activation**: Automatic service registration
4. **Local Storage**: Per-renderer VTK pipeline objects
5. **Service Ranking**: Priority-based format selection

## Usage Example

```cpp
// Reading a fiber bundle (automatic format detection)
mitk::FiberBundle::Pointer bundle = ReadFiberBundle("fibers.trk");

// Creating appropriate mappers
mitk::DataNode::Pointer node = mitk::DataNode::New();
node->SetData(bundle);
// Factory automatically creates FiberBundleMapper2D and FiberBundleMapper3D

// Writing to different format
WriteFiberBundle(bundle, "fibers.tck");
```

## Build Configuration

```cmake
MITK_CREATE_MODULE( DiffusionIO
  INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}
  DEPENDS FiberBundle MitkDICOMReader MitkSceneSerializationBase
  AUTOLOAD_WITH MitkCore
  PACKAGE_DEPENDS PUBLIC DCMTK
)
```

## Key Features

- ✅ Multi-format tractography I/O
- ✅ 2D and 3D fiber visualization
- ✅ Multiple rendering modes (line, tube, ribbon)
- ✅ Color coding support
- ✅ Scene serialization
- ✅ DICOM compliance
- ✅ Automatic format detection
- ✅ Priority-based format selection

## What This Module Does NOT Do

- ❌ Tractography computation/tracking
- ❌ Fiber filtering algorithms (handled by FiberBundle module)
- ❌ DWI image processing
- ❌ Registration or transformation
- ❌ Statistical analysis

## Related Modules

- **FiberBundle**: Core fiber data structures and algorithms
- **ImekaFiber**: Advanced fiber management and filtering
- **ImekaIO**: Additional I/O capabilities for Imeka-specific formats

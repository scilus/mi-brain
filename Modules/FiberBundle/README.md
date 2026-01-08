# FiberBundle Module

## Overview
FiberBundle is the **core data structure module** for tractography in MI-Brain. It provides fundamental classes for representing, manipulating, and processing white matter fiber bundles (streamlines) and diffusion orientation peaks.

## Primary Purpose
- Define core fiber bundle data structures
- Implement fiber manipulation algorithms
- Provide tractography file format parsers
- Calculate fiber-based metrics and statistics
- Support fiber bundle transformations and resampling

## Core Data Structures

### FiberBundle
**Purpose**: Base class for white matter fiber bundles

**Key Features**:
- Inherits from `mitk::BaseData` for MITK integration
- Uses VTK PolyData for efficient geometry storage
- Supports multiple fiber coloring schemes
- Provides geometric transformations

**Color Coding Methods**:
- **Orientation-based**: RGB color by principal fiber direction
- **Curvature-based**: Color by local fiber curvature
- **Length-based**: Color by streamline length
- **Scalar map**: Color from overlaid anatomical images (FA, MD, etc.)
- **Uniform**: Single color for all fibers
- **Opacity mapping**: From scalar values (e.g., FA)

**Manipulation Operations**:
- `ResampleSpline()`: Resample fibers with specified point distance
- `CompressFibers()`: Reduce point count while preserving shape
- `MirrorFibers()`: Mirror across specified axis
- `RemoveShortFibers()`: Filter by minimum length
- `RemoveLongFibers()`: Filter by maximum length
- `SubsampleFibers()`: Random subsampling

**Geometric Queries**:
- Bounding box calculation
- Fiber length statistics
- Point count information
- Geometry updates

### FilteredFiberBundle
**Purpose**: Extended fiber bundle with advanced filtering and coloring

**Additional Color Coding**:
- **EndPoint Coloring**: Color by fiber endpoint locations
- **From File**: Load colors from external file
- **From Anatomy**: Sample colors from anatomical images

**Key Features**:
- Filter tracking and management
- Visibility control per filter
- Statistics calculation on visible fibers
- Real-time filtering updates
- Mirroring with anatomical geometry reference

**Advanced Operations**:
- `ExportDataTo()`: Export visible fibers only
- `CalculateStatsUsingVisibility()`: Compute statistics on filtered set
- `ApplyColorCoding()`: Apply advanced coloring schemes
- `GetRealMaxFiberLength()`: Get maximum length of visible fibers

### PeakImage
**Purpose**: Representation of diffusion orientation distribution peaks

**Key Features**:
- Multi-peak per voxel support
- Direction vectors storage
- Amplitude information
- Integration with MITK image framework

**Use Cases**:
- Fiber orientation distribution visualization
- Tractography seeding
- Diffusion model representation

## File Format Support

### TrackVis (.trk)
**Parser**: `mitkTrackvis.h/cpp`

**Features**:
- Header parsing with voxel-to-RAS transform
- Property extraction (voxel dimensions, image dimensions)
- Coordinate system information
- Scalar data per point support

**Key Classes**:
- `TrkProperties`: TrackVis header information
- Voxel-to-RAS transformation matrix
- Image dimension metadata

### MRtrix TCK (.tck)
**Parser**: `Tck.h/cpp`

**Features**:
- Binary format reading
- Efficient streamline storage
- Property extraction from TCK header
- Coordinate system handling

### VTK PolyData (.vtk, .fib)
**Support**: Native VTK format handling

**Features**:
- Flexible property storage
- Point and cell data support
- Color information preservation

## Utility Classes

### DataStorageUtils
**Purpose**: Helper functions for MITK DataStorage operations

**Capabilities**:
- Finding nodes by predicate
- Node creation and management
- Data type checking
- Hierarchy management

### itkTractDensityImageFilter
**Purpose**: ITK filter for creating tract density images

**Features**:
- Generate 3D density maps from fiber bundles
- Binary or weighted density
- Configurable output resolution
- Support for multiple bundles

**Use Cases**:
- Tract-based spatial statistics (TBSS)
- Coverage analysis
- Quality control
- Registration validation

## Algorithms and Processing

### Fiber Resampling
- **Spline-based resampling**: Smooth interpolation with adjustable tension
- **Fixed point distance**: Uniform sampling along fibers
- **Compression**: Reduce points while preserving geometry

### Fiber Transformation
- **Affine transformations**: Translation, rotation, scaling
- **Mirroring**: Flip across anatomical axes
- **Geometry updates**: Synchronize with reference space

### Statistical Calculations
- Fiber length distribution
- Point density maps
- Coverage metrics
- Bundle statistics (min, max, mean length)

### Color Mapping
- **LUT-based**: Lookup table mapping for scalar values
- **RGB computation**: Direction-based RGB encoding
- **Opacity**: Alpha channel from scalar maps
- **Custom**: User-defined color schemes

## Dependencies

- **MitkCore**: MITK core framework
- **Qt5|Core**: Qt core functionality
- **OpenMP**: Parallel processing support
- **VTK**: Visualization and geometry processing

## Build Configuration

```cmake
MITK_CREATE_MODULE( FiberBundle
  PACKAGE_DEPENDS PUBLIC Qt5|Core OpenMP|OpenMP_CXX
  DEPENDS MitkCore
  WARNINGS_NO_ERRORS
)
```

## Architecture Patterns

1. **Inheritance**: FiberBundle → FilteredFiberBundle hierarchy
2. **VTK Integration**: Uses vtkPolyData for efficient storage
3. **MITK Integration**: Inherits from BaseData
4. **Template Pattern**: Scalar map coloring with template method
5. **Iterator Pattern**: Fiber-by-fiber processing

## Usage Examples

### Basic Fiber Bundle Creation
```cpp
// Create from VTK polydata
vtkSmartPointer<vtkPolyData> polyData = CreateFibers();
mitk::FiberBundle::Pointer bundle = mitk::FiberBundle::New(polyData);

// Color by orientation
bundle->ColorFibersByOrientation();

// Resample fibers
bundle->ResampleSpline(1.0); // 1mm point distance
```

### Advanced Filtering
```cpp
mitk::FilteredFiberBundle::Pointer filtered = 
    mitk::FilteredFiberBundle::New(polyData);

// Apply filters and get statistics
filtered->CalculateStatsUsingVisibility();
float maxLength = filtered->GetRealMaxFiberLength();

// Export visible fibers only
vtkSmartPointer<vtkCellArray> lines;
vtkSmartPointer<vtkPoints> points;
filtered->ExportDataTo(lines, points);
```

### Tract Density Map
```cpp
typedef itk::Image<unsigned char, 3> UCharImage;
itk::TractDensityImageFilter<UCharImage>::Pointer filter = 
    itk::TractDensityImageFilter<UCharImage>::New();
filter->SetInput(bundle);
filter->SetOutputRegion(region);
filter->Update();
UCharImage::Pointer densityMap = filter->GetOutput();
```

## Key Features

- ✅ Efficient fiber storage using VTK PolyData
- ✅ Multiple color coding schemes
- ✅ Fiber manipulation and transformation
- ✅ Filtering and subsampling
- ✅ Multi-format support (TrackVis, TCK, VTK)
- ✅ Parallel processing with OpenMP
- ✅ Statistical calculations
- ✅ Tract density mapping
- ✅ Peak image support

## What This Module Does NOT Do

- ❌ Tractography tracking algorithms (use external tools)
- ❌ File I/O (handled by DiffusionIO module)
- ❌ Visualization/rendering (handled by mapper classes)
- ❌ User interface (handled by ImekaFiber module)
- ❌ Interactive filtering (handled by ImekaFiber module)

## Related Modules

- **DiffusionIO**: File I/O and visualization mappers
- **ImekaFiber**: Advanced fiber management, filtering UI, and interactive tools
- **ImekaGeometry**: Geometric utilities

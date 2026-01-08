# ImekaFiber Module

## Overview
ImekaFiber is the **advanced fiber management and filtering module** for MI-Brain. It provides sophisticated tools for interactive fiber bundle filtering, coloring, manipulation, ROI-based selection, and custom visualization through enhanced mappers.

## Primary Purpose
- Advanced fiber bundle filtering with multiple criteria
- ROI-based fiber selection (inclusion/exclusion)
- Interactive fiber manipulation and editing
- Custom 2D/3D fiber visualization
- Fiber statistics and analysis
- Group management for fiber bundles
- Export functionality for various use cases

## Core Components

### Fiber Management

#### FibersManager
**Purpose**: Central manager for fiber bundle operations and visualization

**Responsibilities**:
- Manage fiber visibility and properties
- Handle mapper creation and updates
- Coordinate filtering operations
- Manage callbacks for property changes
- Synchronize 2D and 3D views

**Key Features**:
- Automatic mapper selection
- Property synchronization
- Visibility management
- Color coding coordination
- Real-time updates

#### FiberNodeData
**Purpose**: Extended data storage for fiber nodes

**Features**:
- Additional metadata storage
- Filtering state
- Display properties
- Statistics cache

### Filtering System

Located in `Filtering/` subdirectory:

#### Filtering
**Purpose**: Core filtering engine

**Filter Types**:
- **Length-based**: Min/max fiber length
- **Curvature-based**: Angular thresholds
- **Distance-based**: Spatial proximity
- **ROI-based**: Region of interest inclusion/exclusion
- **Statistical**: Outlier detection
- **Custom**: User-defined criteria

**Features**:
- Real-time filtering
- Multiple filter combination (AND/OR logic)
- Filter persistence
- Undo/redo support

#### SOFiltering (Streamline Object Filtering)
**Purpose**: Advanced streamline-specific filtering

**Capabilities**:
- Geometric property filtering
- Topology-based selection
- Shape analysis
- Bundle segmentation

#### SurfaceFiltering
**Purpose**: Fiber filtering based on surface interactions

**Features**:
- Surface intersection detection
- Distance to surface calculations
- Surface-constrained selection
- Cortical termination filtering

#### ActiveROIsDialogBox
**Purpose**: Interactive ROI management dialog

**Features**:
- ROI creation and editing
- Multiple ROI operations
- Inclusion/exclusion modes
- Visual feedback
- ROI persistence

**Operations**:
- AND: Fibers pass through all ROIs
- OR: Fibers pass through any ROI
- NOT: Fibers avoid specified ROIs
- End-in: Fibers terminate in ROI

#### FilteringUI
**Purpose**: User interface for filtering controls

**Components**:
- Slider controls for thresholds
- Checkbox filters
- ROI selection
- Real-time preview
- Filter presets

### Visualization Mappers

Located in `Mapper/` subdirectory:

#### MitkFiberMapper2D
**Purpose**: Enhanced 2D fiber visualization

**Features**:
- Slice-plane intersection rendering
- vtkCutter-based geometric slicing
- Per-slice color consistency
- Visibility forcing for MITK 2025
- Optimized for real-time updates

**MITK 2025 Fixes**:
- Forced visibility at multiple pipeline stages
- vtkCutter geometric slicing (replaces disabled shaders)
- Per-renderer visibility management

#### MitkFiberMapper3D
**Purpose**: Enhanced 3D fiber rendering

**Features**:
- Multiple rendering modes (line, tube, ribbon)
- Advanced color coding
- Transparency support
- Clipping planes
- LOD (Level of Detail) support

**MITK 2025 Fixes**:
- VTK actor visibility forcing
- Assembly visibility management
- Shader compatibility updates

#### Mappers2DSettingsWidget
**Purpose**: UI for 2D mapper settings

**Controls**:
- Line width
- Color scheme selection
- Visibility toggles
- Slice plane settings

#### vtkFiberMapper
**Purpose**: Custom VTK mapper for fiber rendering

**Features**:
- Optimized rendering pipeline
- Custom shaders (VTK 8)
- Batch rendering
- GPU acceleration

#### MapperData
**Purpose**: Per-renderer data storage for mappers

**Content**:
- VTK pipeline objects
- Cached geometry
- Update timestamps
- Rendering properties

### Coloring System

Located in `Coloring/` subdirectory:

**Color Schemes**:
- Orientation-based (RGB by direction)
- Endpoint-based (color by termination location)
- Curvature-based (angular changes)
- Length-based (streamline length)
- Anatomical (from underlying images)
- Uniform (single color)
- From file (custom color data)

**Features**:
- Real-time color updates
- Smooth color transitions
- Color map customization
- Opacity control

### Advanced Features

#### RTT (Real-Time Tractography)
**Purpose**: Export fibers for external visualization

**Features**:
- Optimized export format
- Selective fiber export
- Color preservation
- Geometry simplification

#### StreamlinesCutter
**Purpose**: Fiber cutting and trimming operations

**Features**:
- Plane-based cutting
- ROI-based trimming
- Length-based truncation
- Endpoint adjustment

#### Surfaces
**Purpose**: Fiber-surface interaction utilities

**Features**:
- Surface proximity calculations
- Intersection detection
- Cortical termination analysis
- Surface-based ROIs

#### GroupNodes
**Purpose**: Hierarchical grouping of fiber bundles

**Features**:
- Bundle organization
- Group operations
- Hierarchy management
- Visual grouping

#### MaximaData
**Purpose**: Diffusion maxima handling

**Features**:
- Peak extraction
- Direction analysis
- Amplitude information
- Tractography seed generation

### Glyph System

Located in `Glyph/` subdirectory:

**Purpose**: Custom glyph rendering for fibers and peaks

**Features**:
- Tensor glyphs
- Peak direction glyphs
- Custom shapes
- Scaling and coloring

### Persistence

#### Saver
**Purpose**: Save fiber bundles and filtering state

**Features**:
- Scene persistence
- Filter configuration saving
- Color scheme preservation
- ROI state saving

## Dependencies

- **MitkCore**: MITK core framework
- **MitkTestingHelper**: Testing utilities
- **MitkDataTypesExt**: Extended data types
- **MitkPersistence**: Data persistence
- **MitkSegmentation**: Segmentation tools
- **FiberBundle**: Core fiber data structures
- **ImekaBoundingObject**: ROI functionality
- **ImekaCommon**: Common utilities
- **ImekaGeometry**: Geometric operations
- **ImekaWidgets**: UI components
- **Qt5|Core**: Qt functionality

## Build Configuration

```cmake
MITK_CREATE_MODULE( ImekaFiber
  INCLUDE_DIRS ${CMAKE_BINARY_DIR}
  PACKAGE_DEPENDS Qt5|Core
  DEPENDS MitkCore MitkTestingHelper MitkDataTypesExt 
          MitkPersistence MitkSegmentation FiberBundle 
          ImekaBoundingObject ImekaCommon ImekaGeometry 
          ImekaWidgets
  WARNINGS_NO_ERRORS
)
```

## Architecture Patterns

1. **Manager Pattern**: FibersManager coordinates operations
2. **Strategy Pattern**: Multiple filtering strategies
3. **Observer Pattern**: Property change callbacks
4. **Factory Pattern**: Mapper creation
5. **Composite Pattern**: Filter combination
6. **Command Pattern**: Filtering operations

## Usage Examples

### Interactive Filtering
```cpp
// Create filtered bundle
mitk::FilteredFiberBundle::Pointer bundle = LoadFibers("tract.trk");

// Apply length filter
filter->SetMinLength(20.0);
filter->SetMaxLength(150.0);
filter->Apply(bundle);

// Apply ROI filter
filter->AddInclusionROI(roi1);
filter->AddExclusionROI(roi2);
filter->Apply(bundle);
```

### Custom Coloring
```cpp
// Color by endpoints
bundle->ColorCodingByEndPoints();

// Color by curvature
bundle->ColorFibersByCurvature(true);

// Apply custom color map
bundle->ApplyColorCoding(node, anatNode, 
    FilteredFiberBundle::Coloring::FromAnat);
```

### ROI-Based Selection
```cpp
// Create ROI dialog
Imeka::Fiber::ActiveROIsDialogBox dialog;
dialog.AddROI(roi, ActiveROIsDialogBox::Mode::AND);
dialog.AddROI(exclusionROI, ActiveROIsDialogBox::Mode::NOT);

// Apply ROI filtering
auto filtered = dialog.FilterFibers(bundle);
```

## Key Features

- ✅ Advanced multi-criteria filtering
- ✅ ROI-based fiber selection
- ✅ Interactive editing tools
- ✅ Custom 2D/3D visualization
- ✅ Real-time updates
- ✅ Multiple color coding schemes
- ✅ Surface interaction analysis
- ✅ Group management
- ✅ Export functionality
- ✅ MITK 2025 compatibility fixes

## MITK 2025 Migration Fixes

**Visibility Issues**:
- Forced visibility at mapper Update() method
- Forced visibility in GenerateDataForRenderer()
- VTK actor visibility forcing
- Per-renderer visibility management

**2D Slicing**:
- Implemented vtkCutter geometric slicing
- Replaced disabled shader-based approach
- Plane geometry extraction
- Real-time slice updates

## Module Organization

```
ImekaFiber/
├── FibersManager              # Central manager
├── FiberNodeData              # Extended node data
├── Filtering/                 # Filtering system
│   ├── Filtering              # Core filtering
│   ├── SOFiltering            # Streamline filtering
│   ├── SurfaceFiltering       # Surface-based
│   ├── ActiveROIsDialogBox    # ROI management
│   └── FilteringUI            # UI controls
├── Mapper/                    # Custom mappers
│   ├── MitkFiberMapper2D      # 2D rendering
│   ├── MitkFiberMapper3D      # 3D rendering
│   ├── vtkFiberMapper         # VTK mapper
│   └── Mappers2DSettingsWidget
├── Coloring/                  # Color schemes
├── Glyph/                     # Glyph rendering
├── RTT                        # Export utilities
├── StreamlinesCutter          # Cutting operations
├── Surfaces                   # Surface interaction
├── GroupNodes                 # Hierarchy management
└── Saver                      # Persistence
```

## Testing

Comprehensive test suite in `Testing/` subdirectory:
- DuplicateTGTest
- MinMaxFilteringTest
- MirrorTest
- SaveLoadScenesTest
- SimpleFilteringTest
- StatsTest

## What This Module Does NOT Do

- ❌ Tractography computation (use external tracking tools)
- ❌ Basic file I/O (handled by DiffusionIO)
- ❌ Core fiber data structures (handled by FiberBundle)
- ❌ Basic geometric utilities (handled by ImekaGeometry)

## Related Modules

- **FiberBundle**: Core fiber data structures and algorithms
- **DiffusionIO**: File I/O and basic visualization
- **ImekaBoundingObject**: ROI definitions
- **ImekaGeometry**: Geometric utilities
- **ImekaWidgets**: UI components

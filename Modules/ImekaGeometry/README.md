# ImekaGeometry Module

## Overview
ImekaGeometry provides **geometric utilities and helper functions** for spatial operations, coordinate transformations, bounding box calculations, and image/view manipulation in MI-Brain. This module bridges MITK, VTK, and ITK coordinate systems.

## Primary Purpose
- Bounding box calculations and transformations
- Image geometry utilities
- View and camera manipulation
- Mathematical operations for 3D geometry
- VTK image processing utilities
- Coordinate system conversions

## Core Components

### BoundingBoxUtils
**Purpose**: Utilities for bounding box calculations and transformations

**Key Features**:
- **Bounding Box Computation**: Calculate bounds from various data types
  - Point sets
  - Surfaces
  - Images
  - Multiple nodes combined
  
- **Coordinate Transformations**:
  - World to index coordinates
  - Index to world coordinates
  - Affine transformations
  - Geometry-based conversions

- **Spatial Operations**:
  - Union of multiple bounding boxes
  - Intersection testing
  - Expansion/padding
  - Center point calculation

- **Volume Calculations**:
  - Bounding box volume
  - Overlap volume
  - Containment testing

**Common Functions**:
```cpp
// Calculate bounding box from nodes
mitk::BoundingBox::Pointer GetBoundingBox(
    const std::vector<mitk::DataNode*>& nodes);

// Transform bounding box
void TransformBoundingBox(
    mitk::BoundingBox* bbox,
    const mitk::AffineTransform3D* transform);

// Check if point is inside bounds
bool IsPointInside(
    const mitk::Point3D& point,
    const mitk::BoundingBox* bbox);

// Compute union of bounding boxes
mitk::BoundingBox::Pointer UnionBoundingBoxes(
    const std::vector<mitk::BoundingBox::Pointer>& boxes);
```

### MitkImageUtils
**Purpose**: MITK image manipulation and geometry utilities

**Capabilities**:
- **Image Geometry**:
  - Get image spacing
  - Get image origin
  - Get image orientation
  - Calculate image bounds
  
- **Coordinate Conversion**:
  - World to voxel coordinates
  - Voxel to world coordinates
  - Physical to index
  - Index to physical

- **Image Properties**:
  - Dimensions and size
  - Data type information
  - Geometry matrix access
  - Spacing and origin

- **Validation**:
  - Check image validity
  - Verify geometry consistency
  - Dimension compatibility

**Common Operations**:
```cpp
// Get image geometry
auto geometry = MitkImageUtils::GetGeometry(image);

// Convert world to voxel
mitk::Point3D worldPoint;
itk::Index<3> voxelIndex = 
    MitkImageUtils::WorldToIndex(worldPoint, image);

// Get image bounds in world coordinates
auto bounds = MitkImageUtils::GetWorldBounds(image);

// Check if point is inside image
bool inside = MitkImageUtils::IsPointInImage(point, image);
```

### ViewUtils
**Purpose**: Camera and view manipulation utilities

**Key Features**:
- **Camera Operations**:
  - Position camera for optimal viewing
  - Set camera focal point
  - Adjust camera distance
  - Orient camera to specific views (axial, coronal, sagittal)

- **View Initialization**:
  - Initialize render windows
  - Set up 2D/3D views
  - Configure slice navigation
  - Reset view to data bounds

- **Interaction**:
  - Enable/disable interaction
  - Set interaction mode
  - Configure zoom and pan
  - Handle mouse events

- **Display Updates**:
  - Request render updates
  - Force immediate rendering
  - Synchronize multiple views
  - Update level window

**MITK 2025 Fixes**:
Contains fixes for new rendering system compatibility

**Common Functions**:
```cpp
// Initialize view for data
ViewUtils::InitializeView(renderWindow, dataNode);

// Set camera to show all data
ViewUtils::FitCameraToData(renderWindow, dataStorage);

// Set standard view orientation
ViewUtils::SetViewOrientation(renderWindow, ViewOrientation::AXIAL);

// Update display
ViewUtils::RequestUpdate(renderWindow);
```

### MathUtils
**Purpose**: Mathematical utilities for 3D geometry

**Features**:
- **Vector Operations**:
  - Dot product
  - Cross product
  - Normalization
  - Length and distance

- **Matrix Operations**:
  - Matrix multiplication
  - Inverse calculation
  - Transformation matrices
  - Rotation matrices

- **Geometric Calculations**:
  - Point-to-line distance
  - Point-to-plane distance
  - Line-plane intersection
  - Angle between vectors

- **Interpolation**:
  - Linear interpolation
  - Spline interpolation
  - Cubic interpolation

**Common Operations**:
```cpp
// Normalize vector
mitk::Vector3D normalized = MathUtils::Normalize(vector);

// Calculate distance
double dist = MathUtils::Distance(point1, point2);

// Create rotation matrix
vnl_matrix<double> rotation = 
    MathUtils::CreateRotationMatrix(angle, axis);

// Interpolate points
mitk::Point3D interpolated = 
    MathUtils::LinearInterpolate(p1, p2, t);
```

### VtkImageUtils
**Purpose**: VTK image processing utilities

**Capabilities**:
- **Image Conversion**:
  - MITK to VTK image conversion
  - VTK to MITK image conversion
  - Preserve geometry information
  
- **Image Processing**:
  - Resampling
  - Cropping
  - Padding
  - Threshold operations

- **Data Access**:
  - Get image data pointer
  - Access voxel values
  - Iterate over regions
  
- **Geometry Handling**:
  - Extract VTK matrix
  - Apply transformations
  - Update image information

**Common Functions**:
```cpp
// Convert MITK to VTK
vtkSmartPointer<vtkImageData> vtkImage = 
    VtkImageUtils::MitkToVtkImage(mitkImage);

// Resample image
auto resampled = VtkImageUtils::ResampleImage(
    vtkImage, newSpacing, interpolationMode);

// Get voxel value
double value = VtkImageUtils::GetVoxelValue(
    vtkImage, x, y, z);
```

## Dependencies

- **MitkQtWidgets**: Qt-based rendering widgets
- **ImekaCommon**: Common utilities
- **Qt5|Core**: Qt framework
- **VTK**: Via MITK (implicit)
- **ITK**: Via MITK (implicit)

## Build Configuration

```cmake
MITK_CREATE_MODULE( ImekaGeometry
  PACKAGE_DEPENDS Qt5|Core
  DEPENDS MitkQtWidgets ImekaCommon
  WARNINGS_NO_ERRORS
)
```

## Architecture Patterns

1. **Utility Pattern**: Static helper functions
2. **Adapter Pattern**: Coordinate system conversion
3. **Facade Pattern**: Simplified interface to complex operations
4. **Singleton-like**: Header-only utilities (MathUtils, VtkImageUtils)

## Usage Examples

### Calculate Combined Bounding Box
```cpp
// Get all fiber nodes
std::vector<mitk::DataNode*> fiberNodes = GetFiberNodes();

// Calculate combined bounding box
auto bbox = Imeka::Geometry::BoundingBoxUtils::GetBoundingBox(fiberNodes);

// Get bounds
auto bounds = bbox->GetBounds();
std::cout << "X: [" << bounds[0] << ", " << bounds[1] << "]" << std::endl;
std::cout << "Y: [" << bounds[2] << ", " << bounds[3] << "]" << std::endl;
std::cout << "Z: [" << bounds[4] << ", " << bounds[5] << "]" << std::endl;
```

### Position Camera for Visualization
```cpp
// Initialize view to show all data
Imeka::Geometry::ViewUtils::InitializeView(renderWindow, dataStorage);

// Set sagittal orientation
Imeka::Geometry::ViewUtils::SetViewOrientation(
    renderWindow, ViewOrientation::SAGITTAL);

// Fit camera to show all nodes
Imeka::Geometry::ViewUtils::FitCameraToData(
    renderWindow, dataStorage);

// Request render update
Imeka::Geometry::ViewUtils::RequestUpdate(renderWindow);
```

### Convert Coordinates
```cpp
// World point from tractography
mitk::Point3D worldPoint = fiberPoint;

// Convert to image voxel coordinates
auto anatomy = GetAnatomyImage();
itk::Index<3> voxelIdx = 
    Imeka::Geometry::MitkImageUtils::WorldToIndex(worldPoint, anatomy);

// Check if inside image
if (Imeka::Geometry::MitkImageUtils::IsPointInImage(worldPoint, anatomy)) {
    // Get intensity value at location
    auto accessor = anatomy->GetPixelAccessByItk();
    auto value = accessor.GetPixelByIndex(voxelIdx);
    std::cout << "Intensity: " << value << std::endl;
}
```

### Transform Bounding Box
```cpp
// Get fiber bounding box
auto fiberBounds = GetFiberBoundingBox();

// Create transformation matrix (e.g., registration result)
mitk::AffineTransform3D::Pointer transform = GetRegistrationTransform();

// Transform bounding box to new space
Imeka::Geometry::BoundingBoxUtils::TransformBoundingBox(
    fiberBounds.GetPointer(), transform);

// Check if transformed fibers fit in image bounds
auto imageBounds = GetImageBoundingBox();
bool fits = CheckBoundsOverlap(fiberBounds, imageBounds);
```

### Vector Math for Fiber Analysis
```cpp
// Get two fiber segments
mitk::Vector3D segment1 = point2 - point1;
mitk::Vector3D segment2 = point3 - point2;

// Calculate curvature angle
segment1 = Imeka::Geometry::MathUtils::Normalize(segment1);
segment2 = Imeka::Geometry::MathUtils::Normalize(segment2);

double dotProduct = segment1 * segment2; // dot product
double angle = std::acos(dotProduct) * 180.0 / M_PI;

std::cout << "Curvature angle: " << angle << " degrees" << std::endl;
```

## Key Features

- ✅ Bounding box calculations
- ✅ Multi-node bounds union
- ✅ Coordinate system conversions
- ✅ Image geometry utilities
- ✅ Camera and view manipulation
- ✅ Vector and matrix math
- ✅ VTK image processing
- ✅ World ↔ voxel conversions
- ✅ View orientation presets
- ✅ MITK 2025 compatibility
- ✅ Header-only utilities for math
- ✅ Qt integration for rendering

## Module Organization

```
ImekaGeometry/
├── BoundingBoxUtils.cpp/hpp    # Bounding box operations
├── MitkImageUtils.cpp/hpp      # MITK image utilities
├── ViewUtils.cpp/hpp           # Camera and view control
├── MathUtils.hpp               # Mathematical operations (header-only)
└── VtkImageUtils.hpp           # VTK image utilities (header-only)
```

## Common Use Cases

### 1. Visibility Debugging
Calculate bounding boxes to diagnose why data is not visible:
```cpp
auto fiberBounds = GetBoundingBox(fiberNodes);
auto anatBounds = GetBoundingBox(anatomyNodes);

// Check if bounds overlap
bool visible = CheckOverlap(fiberBounds, anatBounds);
if (!visible) {
    std::cout << "Bounds don't overlap - camera issue!" << std::endl;
}
```

### 2. ROI Placement
Use geometry utilities to position ROIs at specific anatomical locations:
```cpp
// Convert anatomical coordinates to world
mitk::Point3D roiCenter = 
    MitkImageUtils::IndexToWorld(atlasIndex, anatomy);

// Place ROI at location
auto roi = CreateSphereROI(roiCenter, radius);
```

### 3. View Initialization
Set up render windows for optimal data visualization:
```cpp
// Initialize all views
ViewUtils::InitializeView(axialView, dataStorage);
ViewUtils::InitializeView(sagittalView, dataStorage);
ViewUtils::InitializeView(coronalView, dataStorage);
ViewUtils::InitializeView(view3D, dataStorage);

// Set orientations
ViewUtils::SetViewOrientation(axialView, ViewOrientation::AXIAL);
ViewUtils::SetViewOrientation(sagittalView, ViewOrientation::SAGITTAL);
ViewUtils::SetViewOrientation(coronalView, ViewOrientation::CORONAL);

// Fit cameras
for (auto view : allViews) {
    ViewUtils::FitCameraToData(view, dataStorage);
}
```

### 4. Coordinate-Based Queries
Query fiber properties at specific anatomical locations:
```cpp
// User clicks on anatomy in 2D view
mitk::Point3D clickPoint = GetClickPoint();

// Convert to voxel coordinates
auto voxelIdx = MitkImageUtils::WorldToIndex(clickPoint, anatomy);

// Find fibers passing through this location
auto roi = CreateSmallROI(clickPoint);
auto fibers = FilterFibersThroughROI(roi);
```

## Integration with Other Modules

### With ImekaFiber
```cpp
// Calculate fiber bundle bounds
auto fiberBounds = BoundingBoxUtils::GetBoundingBox({fiberNode});

// Use for filtering
if (BoundingBoxUtils::IsPointInside(point, fiberBounds)) {
    // Point is within fiber extent
}
```

### With ImekaBoundingObject
```cpp
// Get ROI geometry
auto roiBounds = GetROIBoundingBox();

// Transform to image space
auto transform = GetImageToWorldTransform();
BoundingBoxUtils::TransformBoundingBox(roiBounds, transform);
```

### With DiffusionIO
```cpp
// After loading TRK file, validate geometry
auto geometry = MitkImageUtils::GetGeometry(anatomyImage);
auto fiberBounds = GetFiberBoundingBox();

// Check if fibers are in image space
bool valid = CheckGeometryCompatibility(geometry, fiberBounds);
```

## MITK 2025 Compatibility

**ViewUtils Updates**:
- Compatible with new rendering pipeline
- Updated render window access
- Qt 6 rendering integration
- Thread-safe render requests

**Coordinate System**:
- VTK 9.4 coordinate conventions
- Updated transformation matrices
- Geometry API changes handled

## What This Module Does NOT Do

- ❌ Data storage management (MITK DataStorage)
- ❌ File I/O operations (ImekaIO)
- ❌ Rendering implementation (MITK rendering)
- ❌ User interface (ImekaWidgets)
- ❌ Fiber filtering logic (ImekaFiber)
- ❌ Image registration (MITK registration modules)

## Related Modules

- **ImekaBoundingObject**: Uses geometry utilities for ROI calculations
- **ImekaFiber**: Uses for fiber spatial queries
- **DiffusionIO**: Uses for coordinate validation
- **ImekaWidgets**: Uses ViewUtils for render window setup
- **ImekaCommon**: Provides base utilities
- **MitkQtWidgets**: Provides render window infrastructure

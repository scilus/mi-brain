# ImekaBoundingObject Module

## Overview
ImekaBoundingObject is a module providing **Region of Interest (ROI) management** functionality for MI-Brain. It handles the creation, manipulation, interaction, and visualization of bounding objects used for fiber filtering, segmentation, and spatial analysis.

## Primary Purpose
- Define and manage Region of Interest (ROI) objects
- Provide interactive ROI manipulation tools
- Handle ROI selection and highlighting
- Support various bounding object types
- Enable ROI-based fiber filtering
- Manage ROI node properties and visualization

## Core Components

### BoundingObjectFactory
**Purpose**: Factory for creating bounding object data nodes

**Supported Bounding Object Types**:
- **Sphere**: Spherical ROI
- **Ellipsoid**: Ellipsoidal ROI  
- **Box**: Rectangular box ROI
- **Cylinder**: Cylindrical ROI
- **Cone**: Conical ROI
- **Custom**: User-defined shapes

**Responsibilities**:
- Create appropriate MITK data nodes
- Initialize bounding object properties
- Set default visualization properties
- Register with MITK object factory system

**Features**:
- Automatic property initialization
- Default color schemes
- Size and position defaults
- Interaction enablement

### InteractorEmitter
**Purpose**: Qt-based interactor for ROI selection and manipulation

**Key Features**:
- **Signal Emission**: Qt signals for selection events
  - `Selected(DataNode*)`: Emitted when ROI is selected
  - `Unselected(DataNode*)`: Emitted when ROI is deselected
  
- **Visual Feedback**: 
  - Opacity changes on selection (0.8 when selected)
  - Node highlighting
  - Interactive state tracking

- **State Management**:
  - Track initialization state
  - Handle selection/deselection
  - Manage interaction mode

**MITK 2025 Fixes**:
- Added `(void)action` and `(void)event` to suppress unused parameter warnings
- Compatible with new MITK interaction framework

**Signals**:
```cpp
Q_SIGNALS:
  void Selected(mitk::DataNode* boundingObject);
  void Unselected(mitk::DataNode* boundingObject);
```

**Usage**:
Inherits from QObject for Qt signal/slot mechanism, enabling UI integration for ROI selection feedback.

### NodeUtils
**Purpose**: Utility functions for bounding object node operations

**Capabilities**:
- **Node Creation**: Create nodes with proper initialization
- **Property Management**: Set/get bounding object properties
- **Type Checking**: Identify bounding object types
- **Conversion**: Convert between different ROI representations
- **Validation**: Check ROI validity and constraints

**Common Operations**:
- `CreateBoundingObjectNode()`: Create and initialize node
- `IsBoundingObject()`: Check if node is a bounding object
- `GetBoundingObjectGeometry()`: Extract geometric properties
- `UpdateBoundingObjectVisualization()`: Refresh rendering

## Interaction System

### State Machine Integration
The module integrates with MITK's state machine system for interactive manipulation:

**Interaction Events**:
- Mouse click: Selection
- Mouse drag: Translation
- Mouse wheel: Scaling
- Keyboard: Precise adjustments

**State Transitions**:
1. **Idle**: No interaction
2. **Selected**: ROI is selected, ready for manipulation
3. **Moving**: ROI is being translated
4. **Resizing**: ROI is being scaled
5. **Rotating**: ROI is being rotated

### Visual Feedback

**Selection Indicators**:
- Opacity change (0.8 when selected)
- Color highlighting
- Bounding box display
- Handle visibility

**Interaction Modes**:
- **Translation**: Move ROI in 3D space
- **Rotation**: Rotate around axes
- **Scaling**: Resize uniformly or per-axis
- **Deformation**: Free-form shape adjustment

## Integration with Fiber Filtering

### ROI-Based Fiber Selection

**Use Cases**:
1. **Inclusion ROI**: Select fibers passing through
2. **Exclusion ROI**: Remove fibers passing through
3. **Endpoint ROI**: Select fibers terminating within
4. **Waypoint ROI**: Select fibers connecting ROIs

**Operations**:
- AND: Fibers must pass through all ROIs
- OR: Fibers must pass through at least one ROI
- NOT: Fibers must not pass through specified ROIs
- XOR: Fibers must pass through exactly one ROI

### Spatial Queries

**Geometric Tests**:
- Point-in-ROI testing
- Line-ROI intersection
- Distance to ROI boundary
- Overlap detection

## Dependencies

- **MitkDataTypesExt**: Extended MITK data types for bounding objects
- **MitkQtWidgets**: Qt-based widgets for interaction
- **ImekaCommon**: Common utilities
- **Qt5|Core+Widgets**: Qt framework for signals/slots

## Build Configuration

```cmake
MITK_CREATE_MODULE( ImekaBoundingObject
  PACKAGE_DEPENDS Qt5|Core+Widgets
  DEPENDS MitkDataTypesExt MitkQtWidgets ImekaCommon
  WARNINGS_NO_ERRORS
)
```

## Architecture Patterns

1. **Factory Pattern**: BoundingObjectFactory for creation
2. **Observer Pattern**: InteractorEmitter for event notification
3. **Utility Pattern**: NodeUtils for common operations
4. **State Machine**: MITK state machine integration
5. **Signal/Slot**: Qt mechanism for UI communication

## Usage Examples

### Creating a Bounding Object
```cpp
// Create a sphere ROI
mitk::DataNode::Pointer node = 
    Imeka::BoundingObject::Factory::CreateSphere(center, radius);

// Add to data storage
dataStorage->Add(node);
```

### Interactive Selection
```cpp
// Create interactor
Imeka::BoundingObject::InteractorEmitter* interactor = 
    new Imeka::BoundingObject::InteractorEmitter();

// Connect signals
connect(interactor, &InteractorEmitter::Selected, 
    [](mitk::DataNode* node) {
        qDebug() << "ROI selected:" << node->GetName().c_str();
    });

// Attach to node
node->SetDataInteractor(interactor);
```

### Node Utilities
```cpp
// Check if node is a bounding object
if (Imeka::BoundingObject::NodeUtils::IsBoundingObject(node)) {
    // Get geometry
    auto geometry = NodeUtils::GetBoundingObjectGeometry(node);
    
    // Update visualization
    NodeUtils::UpdateBoundingObjectVisualization(node);
}
```

### ROI-Based Filtering
```cpp
// Get inclusion ROIs
std::vector<mitk::DataNode::Pointer> rois = GetInclusionROIs();

// Filter fibers through ROIs
auto filtered = filterEngine->FilterByROIs(fibers, rois, 
    FilterEngine::Mode::AND);
```

## Key Features

- ✅ Multiple bounding object types
- ✅ Interactive manipulation
- ✅ Qt signal/slot integration
- ✅ Visual selection feedback
- ✅ Fiber filtering integration
- ✅ Spatial query support
- ✅ State machine integration
- ✅ MITK 2025 compatibility
- ✅ Factory pattern for creation
- ✅ Utility functions for common operations

## Module Organization

```
ImekaBoundingObject/
├── BoundingObjectFactory      # Factory for creation
├── InteractorEmitter           # Interactive selection
├── NodeUtils                   # Utility functions
└── Testing/                    # Unit tests
```

## Testing

The module includes comprehensive unit tests for:
- Factory creation
- Interactor functionality
- Node utilities
- Geometric queries

## MITK 2025 Compatibility

**Fixes Applied**:
- Unused parameter warnings suppressed in `InteractorEmitter::InitAction()`
- Compatible with new state machine framework
- Updated signal/slot mechanisms
- Qt 6 compatibility preparation

## Common Use Cases

### 1. Manual Tract Segmentation
Create ROIs to manually delineate white matter tracts:
```cpp
// Place inclusion ROIs at expected tract locations
roi1 = CreateSphere(startLocation, 5.0);
roi2 = CreateSphere(endLocation, 5.0);

// Filter fibers connecting the ROIs
tract = FilterFibersThroughROIs({roi1, roi2}, AND);
```

### 2. Exclusion-Based Cleaning
Remove unwanted fibers using exclusion ROIs:
```cpp
// Create exclusion ROI at artifact location
exclusionROI = CreateBox(artifactLocation, size);

// Remove fibers passing through
cleanedFibers = FilterWithExclusion(fibers, exclusionROI);
```

### 3. Cortical Termination Analysis
Analyze fiber terminations in cortical regions:
```cpp
// Create ROI around cortical region
cortexROI = CreateEllipsoid(cortexCenter, axes);

// Find fibers with endpoints in ROI
terminatingFibers = FilterByEndpoint(fibers, cortexROI);
```

### 4. Virtual Dissection
Interactive tract dissection workflow:
```cpp
// Enable interactive mode
EnableInteractiveROIPlacement();

// User places ROIs interactively
// Fibers update in real-time based on ROI positions
ConnectROIsToFilteringEngine();
```

## What This Module Does NOT Do

- ❌ Fiber filtering logic (handled by ImekaFiber)
- ❌ 3D rendering (handled by MITK rendering system)
- ❌ ROI registration/transformation (handled by MITK registration)
- ❌ Statistical analysis (handled by ImekaReport)
- ❌ File I/O for ROIs (handled by ImekaIO)

## Related Modules

- **ImekaFiber**: Uses bounding objects for fiber filtering
- **ImekaIO**: Handles ROI persistence
- **ImekaWidgets**: Provides UI for ROI management
- **ImekaCommon**: Provides common utilities
- **MitkDataTypesExt**: Provides base bounding object types

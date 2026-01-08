# ImekaCommon Module

## Overview
ImekaCommon is a **shared utilities module** providing common functionality used across all Imeka modules. It contains helper classes for property management, data persistence, color handling, predicates, callbacks, parallel processing, and various utility functions.

## Primary Purpose
- Provide reusable utility classes for all Imeka modules
- Extend MITK property system with custom types
- Handle data persistence and serialization
- Provide color management utilities
- Enable parallel processing helpers
- Define common predicates for node selection

## Core Components

### Property System Extensions

#### Custom Property Types
Located in `Property/` subdirectory:

**Int64Property**
- 64-bit integer property type
- Extends MITK property system
- Serialization support

**UInt64Property**
- Unsigned 64-bit integer property type
- Extends MITK property system
- Serialization support

**Matrix3DProperty**
- 3x3 matrix property type
- Rotation/transformation matrices
- Serialization support

**GenericPropertySerializer**
- Template-based property serializer
- Handles custom property types
- Integration with MITK scene serialization

**Matrix3DPropertySerializer**
- Specialized serializer for Matrix3D properties
- Scene persistence

### Data Management

#### DataManager
**Purpose**: Centralized data management utilities

**Features**:
- Node creation and management
- Data storage operations
- Node hierarchy management
- Data type conversions

#### Persistence
**Purpose**: Data persistence and serialization helpers

**Features**:
- Property persistence
- State saving/loading
- Configuration management
- Session state handling

#### SaveDialog & SaveConfirmation
**Purpose**: User-friendly save dialogs

**Features**:
- Custom save dialogs
- Confirmation prompts
- File format selection
- Overwrite protection

### Color Management

#### Colors
**Purpose**: Color handling and conversion utilities

**Features**:
- RGB/HSV conversions
- Color palette generation
- Predefined color schemes
- Color validation

**Methods**:
- `Apply()`: Apply color to data nodes
- Color space conversions
- Palette creation
- Color interpolation

#### RGBMapper
**Purpose**: RGB value mapping and lookup

**Features**:
- Scalar to RGB mapping
- Custom color maps
- Interpolation
- Normalization

**Use Cases**:
- Anatomical image coloring
- Scalar field visualization
- Custom colormap application

### Predicate System

#### Predicate
**Purpose**: Base predicate utilities for node selection

**Features**:
- Node filtering
- Type checking
- Property-based selection
- Composite predicates

#### mitkNodePredicateImageInfo
**Purpose**: Image-specific node predicate

**Features**:
- Select nodes containing images
- Filter by image properties
- Dimension checking
- Type validation

### Callback System

#### Callback
**Purpose**: Event callback management

**Features**:
- Observer pattern implementation
- Event registration
- Callback chaining
- Lifetime management

**Use Cases**:
- Property change notifications
- Node addition/removal events
- Data modification tracking

### Parallel Processing

#### Parallel
**Purpose**: OpenMP-based parallel processing utilities

**Features**:
- Thread-safe operations
- Parallel loops
- Task scheduling
- Performance optimization

**Use Cases**:
- Batch processing
- Fiber bundle operations
- Image processing
- Statistical calculations

### Utility Headers

#### types.hpp
**Purpose**: Common type definitions

**Content**:
- Type aliases
- Enum definitions
- Constant definitions
- Forward declarations

#### DataTypeUtils.hpp
**Purpose**: Data type checking and conversion

**Features**:
- Type checking functions
- Safe type casting
- Type trait utilities
- MITK data type helpers

#### VtkImageUtils.hpp
**Purpose**: VTK image utilities

**Features**:
- VTK to MITK conversions
- Image format conversions
- Metadata handling
- Geometry utilities

## Dependencies

- **MitkCore**: MITK core framework
- **MitkSceneSerializationBase**: Scene persistence
- **Qt5|Widgets**: Qt widget functionality
- **OpenMP**: Parallel processing

## Build Configuration

```cmake
MITK_CREATE_MODULE( ImekaCommon
  PACKAGE_DEPENDS Qt5|Widgets OpenMP|OpenMP_CXX
  DEPENDS MitkCore MitkSceneSerializationBase
  WARNINGS_NO_ERRORS
)
```

## Architecture Patterns

1. **Utility Pattern**: Static helper functions
2. **Singleton Pattern**: Centralized managers
3. **Observer Pattern**: Callback system
4. **Serializer Pattern**: Property persistence
5. **Template Pattern**: Generic property serialization

## Usage Examples

### Custom Property Usage
```cpp
// Create and set Int64 property
mitk::Int64Property::Pointer prop = mitk::Int64Property::New(12345);
node->SetProperty("fiber_count", prop);

// Retrieve property
int64_t value;
node->GetIntProperty("fiber_count", value);
```

### Color Application
```cpp
// Apply uniform color to node
QColor color(255, 0, 0); // Red
Imeka::Color::Apply(node, color);

// Generate color palette
std::vector<QColor> palette = Imeka::Colors::GeneratePalette(10);
```

### Parallel Processing
```cpp
// Parallel loop over fibers
Imeka::Parallel::For(0, fiberCount, [&](int i) {
    // Process fiber i in parallel
    ProcessFiber(fibers[i]);
});
```

### Predicate Usage
```cpp
// Find all image nodes
mitk::NodePredicateImageInfo::Pointer pred = 
    mitk::NodePredicateImageInfo::New();
auto images = dataStorage->GetSubset(pred);
```

### Persistence
```cpp
// Save properties
Imeka::Persistence::SaveProperties(node, "config.xml");

// Load properties
Imeka::Persistence::LoadProperties(node, "config.xml");
```

## Key Features

- ✅ Extended property system with custom types
- ✅ Color management utilities
- ✅ Parallel processing support
- ✅ Data persistence framework
- ✅ Node predicate system
- ✅ Callback management
- ✅ Type-safe utilities
- ✅ VTK integration helpers
- ✅ Scene serialization support

## Module Organization

```
ImekaCommon/
├── Property/                  # Custom property types
│   ├── Int64Property
│   ├── UInt64Property
│   ├── Matrix3DProperty
│   └── Serializers
├── Callback                   # Event handling
├── Colors                     # Color management
├── RGBMapper                  # Color mapping
├── DataManager                # Data operations
├── Persistence                # Serialization
├── Predicate                  # Node selection
├── Parallel                   # OpenMP utilities
├── SaveDialog                 # UI dialogs
├── types.hpp                  # Type definitions
├── DataTypeUtils.hpp          # Type checking
└── VtkImageUtils.hpp          # VTK helpers
```

## Design Principles

1. **Reusability**: Designed for use across all Imeka modules
2. **Type Safety**: Strong typing with compile-time checks
3. **Performance**: Optimized with parallel processing
4. **Extensibility**: Easy to add new utilities
5. **MITK Integration**: Seamless integration with MITK framework

## What This Module Does NOT Do

- ❌ Application-specific logic (belongs in specialized modules)
- ❌ User interface components (handled by ImekaWidgets)
- ❌ Rendering (handled by mapper modules)
- ❌ File I/O (handled by ImekaIO or DiffusionIO)
- ❌ Geometric algorithms (handled by ImekaGeometry)

## Related Modules

- **ImekaWidgets**: UI components that use these utilities
- **ImekaGeometry**: Geometric utilities
- **ImekaFiber**: Fiber-specific functionality using common utilities
- **ImekaBoundingObject**: Bounding object functionality using common utilities
- All other Imeka modules depend on this module

# ImekaIO Module

## Overview
ImekaIO provides **custom file I/O and serialization** for Imeka-specific data formats in MI-Brain. This module handles reading, writing, and serialization of bounding objects, data node groups, and specialized image formats (FDF, VFF).

## Primary Purpose
- Read/write bounding object files
- Read/write data node group files
- Load Varian FDF images (Magnetic Resonance Imaging format)
- Load VFF (Visualization File Format) images
- Provide MIME type support for custom formats
- Auto-register readers/writers with MITK I/O system

## Core Components

### BoundingObject I/O

#### BoundingObjectReader
**Purpose**: Load bounding object files from disk

**Supported Formats**:
- `.bobj`: Imeka bounding object format
- Custom XML-based format

**Features**:
- Deserialize bounding object geometry
- Restore properties and metadata
- Reconstruct transformation matrices
- Load multiple objects from single file

**Usage**:
```cpp
Imeka::IO::BoundingObjectReader reader;
std::vector<mitk::BaseData::Pointer> objects = 
    reader.Read("/path/to/roi.bobj");
```

#### BoundingObjectWriter
**Purpose**: Save bounding object files to disk

**Features**:
- Serialize bounding object geometry
- Save properties and metadata
- Export transformation matrices
- Support batch writing

**Usage**:
```cpp
Imeka::IO::BoundingObjectWriter writer;
writer.SetInput(boundingObject);
writer.SetOutputLocation("/path/to/roi.bobj");
writer.Write();
```

#### BoundingObjectSerializer
**Purpose**: Handle serialization/deserialization for MITK scene saving

**Integration**:
- Registered with MITK object factory
- Automatic serialization when saving scenes
- Preserves all properties
- Compatible with MITK scene format

### DataNodeGroup I/O

#### DataNodeGroupReader
**Purpose**: Load grouped data nodes from files

**Features**:
- Load multiple nodes with relationships
- Preserve parent-child hierarchies
- Restore visibility states
- Load associated properties

**Use Cases**:
- Load pre-defined tract packages
- Load anatomical region sets
- Load curated fiber bundles
- Load multi-object ROI sets

**File Format**:
Custom format grouping multiple data nodes with metadata

**Usage**:
```cpp
Imeka::IO::DataNodeGroupReader reader;
auto nodes = reader.Read("/path/to/tract_package.dng");

// Nodes are organized in hierarchical structure
for (auto node : nodes) {
    dataStorage->Add(node);
}
```

#### DataNodeGroupWriter
**Purpose**: Save grouped data nodes to files

**Features**:
- Export multiple nodes together
- Preserve hierarchies
- Save metadata
- Support versioning

**Use Cases**:
- Save custom tract packages
- Export ROI sets
- Create reusable bundles
- Share curated data

**Usage**:
```cpp
Imeka::IO::DataNodeGroupWriter writer;
writer.SetInput(nodes);
writer.SetOutputLocation("/path/to/tract_package.dng");
writer.Write();
```

#### DataNodeGroupSerializer
**Purpose**: Scene serialization support

**Integration**:
- Automatic scene saving
- Preserves node relationships
- Restores hierarchies on load

### FDF Image Loader

#### FDFReader
**Purpose**: Load Varian FDF (Flexible Data Format) images

**Format Details**:
- **Origin**: Varian MRI scanners
- **Structure**: Text header + binary data
- **Common Use**: Pre-clinical MRI (animal imaging)

**Supported Features**:
- Multi-slice volumes
- Diffusion-weighted images
- Gradient table extraction
- Scanner parameters

**Header Information**:
- Spatial dimensions
- Voxel spacing
- Image orientation
- Acquisition parameters
- Gradient directions (for DWI)

**Usage**:
```cpp
Imeka::IO::FDFReader reader;
mitk::Image::Pointer image = reader.Read("/path/to/data.fdf");

// Access gradient information for DWI
if (reader.HasGradients()) {
    auto gradients = reader.GetGradients();
    auto bValues = reader.GetBValues();
}
```

**MIME Type**: `application/vnd.imeka.fdf`

### VFF Image Loader

#### VFFReader
**Purpose**: Load VFF (Visualization File Format) images

**Format Details**:
- **Origin**: Khoros Visualization Framework
- **Structure**: ASCII header + binary data
- **Common Use**: Legacy visualization data

**Supported Features**:
- 2D and 3D images
- Multiple data types (uint8, uint16, float)
- Spatial information
- Multi-band images

**Header Fields**:
- rank (dimensionality)
- size (dimensions)
- bands (channels)
- bits (data type)
- spacing (voxel size)

**Usage**:
```cpp
Imeka::IO::VFFReader reader;
mitk::Image::Pointer image = reader.Read("/path/to/data.vff");
```

**MIME Type**: `application/vnd.imeka.vff`

## MIME Type System

### MimeType
**Purpose**: Define custom MIME types for Imeka formats

**Registered MIME Types**:
- `application/vnd.imeka.boundingobject`: Bounding object files (.bobj)
- `application/vnd.imeka.datanodegroup`: Data node group files (.dng)
- `application/vnd.imeka.fdf`: Varian FDF images (.fdf)
- `application/vnd.imeka.vff`: VFF images (.vff)

**Integration**:
- Registered with MITK MIME type system
- Used for file type detection
- Enables drag-and-drop support
- Supports file dialog filters

## Module Activator

### ImekaIOModuleActivator
**Purpose**: Auto-registration of I/O services

**Responsibilities**:
- Register all readers with MITK I/O service
- Register all writers with MITK I/O service
- Register serializers with object factory
- Register MIME types
- Execute on module load (AUTOLOAD_WITH MitkCore)

**Auto-registered Services**:
1. BoundingObjectReader
2. BoundingObjectWriter
3. BoundingObjectSerializer
4. DataNodeGroupReader
5. DataNodeGroupWriter
6. DataNodeGroupSerializer
7. FDFReader
8. VFFReader

**Activation**:
Automatically activated when MITK core loads - no manual initialization required.

## Dependencies

- **MitkPlanarFigure**: Base classes for bounding objects
- **ImekaCommon**: Common utilities and properties
- **ImekaWidgets**: UI integration for I/O dialogs
- **AUTOLOAD_WITH MitkCore**: Automatic activation

## Build Configuration

```cmake
MITK_CREATE_MODULE( ImekaIO
  PACKAGE_DEPENDS
  DEPENDS MitkPlanarFigure ImekaCommon ImekaWidgets
  AUTOLOAD_WITH MitkCore
  WARNINGS_NO_ERRORS
)
```

**Key Feature**: `AUTOLOAD_WITH MitkCore` ensures all readers/writers are automatically available when MI-Brain starts.

## Architecture Patterns

1. **Reader/Writer Pattern**: Separate read and write operations
2. **Serializer Pattern**: MITK scene integration
3. **Factory Pattern**: Automatic service registration
4. **Module Activator Pattern**: Auto-registration on load
5. **MIME Type System**: File type identification

## Usage Examples

### Save and Load ROI
```cpp
// Create ROI
auto roi = CreateSphereROI(center, radius);

// Save to file
Imeka::IO::BoundingObjectWriter writer;
writer.SetInput(roi);
writer.SetOutputLocation("/path/to/roi.bobj");
writer.Write();

// Load from file
Imeka::IO::BoundingObjectReader reader;
auto loaded = reader.Read("/path/to/roi.bobj");
dataStorage->Add(loaded[0]);
```

### Load Varian FDF Image
```cpp
// Load FDF file
Imeka::IO::FDFReader reader;
mitk::Image::Pointer image = reader.Read("/path/to/scan.fdf");

// Add to data storage
auto node = mitk::DataNode::New();
node->SetData(image);
node->SetName("MRI Scan");
dataStorage->Add(node);

// Access DWI gradients if available
if (reader.HasGradients()) {
    auto gradients = reader.GetGradients();
    std::cout << "Found " << gradients.size() << " gradients" << std::endl;
}
```

### Create Tract Package
```cpp
// Select tract bundles to package
std::vector<mitk::DataNode::Pointer> tracts = {
    cstLeft, cstRight, corpus_callosum, arcuate
};

// Save as grouped package
Imeka::IO::DataNodeGroupWriter writer;
writer.SetInput(tracts);
writer.SetOutputLocation("/path/to/major_tracts.dng");
writer.Write();

// Load package later
Imeka::IO::DataNodeGroupReader reader;
auto loadedTracts = reader.Read("/path/to/major_tracts.dng");
for (auto tract : loadedTracts) {
    dataStorage->Add(tract);
}
```

### Automatic Scene Saving
```cpp
// Bounding objects are automatically saved with scenes
// No explicit serialization code needed

// User saves scene -> BoundingObjectSerializer invoked
// User loads scene -> BoundingObjectSerializer invoked
// All properties and geometry restored automatically
```

## Key Features

- ✅ Custom file format support
- ✅ Bounding object persistence
- ✅ Data node grouping
- ✅ Varian FDF image loading
- ✅ VFF image loading
- ✅ Automatic service registration
- ✅ MIME type integration
- ✅ Scene serialization support
- ✅ Multi-object file support
- ✅ Gradient table extraction (FDF)
- ✅ Drag-and-drop support via MIME types
- ✅ MITK I/O framework integration

## Module Organization

```
ImekaIO/
├── BoundingObject/
│   ├── BoundingObjectReader.cpp/hpp      # Read .bobj files
│   ├── BoundingObjectWriter.cpp/hpp      # Write .bobj files
│   └── BoundingObjectSerializer.cpp/hpp  # Scene integration
├── DataNodeGroup/
│   ├── DataNodeGroupReader.cpp/hpp       # Read .dng files
│   ├── DataNodeGroupWriter.cpp/hpp       # Write .dng files
│   └── DataNodeGroupSerializer.cpp/hpp   # Scene integration
├── FDFLoader/
│   └── FDFReader.cpp/hpp                 # Varian FDF loader
├── VFFLoader/
│   └── VFFReader.cpp/hpp                 # VFF loader
├── ImekaIOModuleActivator.cpp            # Auto-registration
└── MimeType.cpp/hpp                      # MIME type definitions
```

## File Format Details

### .bobj (Bounding Object)
**Structure**:
- XML header with metadata
- Geometry data (type, parameters)
- Transformation matrix
- Properties (color, opacity, name)

**Example Use Cases**:
- Save manually placed ROIs
- Share ROI definitions between users
- Reuse ROIs across sessions

### .dng (Data Node Group)
**Structure**:
- List of data nodes with relationships
- Parent-child hierarchy
- Per-node properties
- Group metadata

**Example Use Cases**:
- Distribute pre-segmented tract bundles
- Share curated datasets
- Create reusable analysis templates

### .fdf (Flexible Data Format)
**Structure**:
- ASCII header with key-value pairs
- Binary image data
- Optional gradient table
- Scanner parameters

**Common Parameters**:
- `spatial_rank`: Number of dimensions
- `matrix`: Image dimensions
- `span`: Voxel spacing
- `location`: Image origin
- `diffusion`: Gradient information

### .vff (Visualization File Format)
**Structure**:
- ASCII header with parameters
- Binary data section
- Multi-band support
- Type specification

**Common Fields**:
- `rank`: Dimensionality
- `size`: Dimensions per axis
- `bands`: Number of channels
- `bits`: Bits per pixel
- `spacing`: Voxel size

## Integration with Other Modules

### With ImekaBoundingObject
```cpp
// ImekaBoundingObject creates ROIs
// ImekaIO saves/loads them
auto roi = BoundingObjectFactory::CreateSphere(...);
BoundingObjectWriter::Write(roi, "roi.bobj");
```

### With DiffusionIO
```cpp
// Load anatomy with FDFReader
auto anatomy = FDFReader::Read("anatomy.fdf");

// Load tractography with DiffusionIO
auto fibers = LoadTrkFile("fibers.trk");

// Both in same coordinate space
```

### With ImekaWidgets
```cpp
// ImekaIO provides I/O backend
// ImekaWidgets provides file dialogs
QString filename = QFileDialog::getOpenFileName(
    nullptr, "Open ROI", "", "Bounding Objects (*.bobj)");
    
if (!filename.isEmpty()) {
    auto objects = BoundingObjectReader::Read(filename);
    // ...
}
```

## DICOM Support (Disabled)

The module contains a `Dicom.disabled/` subdirectory indicating DICOM support was previously implemented but is currently disabled, likely in favor of MITK's built-in DICOM support.

## Common Use Cases

### 1. Save Manual Segmentation
```cpp
// User manually places ROIs for tract segmentation
std::vector<mitk::DataNode::Pointer> rois = GetAllROIs();

// Save for future use
for (size_t i = 0; i < rois.size(); ++i) {
    BoundingObjectWriter writer;
    writer.SetInput(rois[i]->GetData());
    writer.SetOutputLocation($"roi_{i}.bobj");
    writer.Write();
}
```

### 2. Load Pre-clinical MRI
```cpp
// Load mouse brain MRI from Varian scanner
FDFReader reader;
auto mouseBrain = reader.Read("/data/mouse_scan.fdf");

// Load corresponding tractography
auto mouseTracts = LoadTrkFile("/data/mouse_fibers.trk");

// Both in scanner coordinate space
```

### 3. Distribute Atlas ROIs
```cpp
// Create atlas ROIs for standard tracts
auto cst_left = CreateCST_Left();
auto cst_right = CreateCST_Right();
// ... more tracts

// Package together
DataNodeGroupWriter writer;
writer.SetInput({cst_left, cst_right, ...});
writer.SetOutputLocation("standard_tracts_atlas.dng");
writer.Write();

// Users can load the package
DataNodeGroupReader reader;
auto atlas = reader.Read("standard_tracts_atlas.dng");
```

### 4. Session Persistence
```cpp
// Bounding objects automatically saved with MITK scenes
// User: File -> Save Scene
// -> BoundingObjectSerializer invoked automatically
// -> All ROIs saved with scene

// User: File -> Load Scene
// -> BoundingObjectSerializer restores ROIs
// -> Exact geometry and properties restored
```

## What This Module Does NOT Do

- ❌ Standard image formats (NIFTI, DICOM) - handled by MITK core
- ❌ Tractography formats (TRK, TCK) - handled by DiffusionIO
- ❌ 3D rendering - handled by MITK rendering
- ❌ UI file dialogs - provided by ImekaWidgets
- ❌ Data processing - handled by respective modules
- ❌ Image registration - handled by MITK registration

## Related Modules

- **ImekaBoundingObject**: Creates ROIs that ImekaIO persists
- **DiffusionIO**: Handles tractography file formats
- **ImekaWidgets**: Provides file dialog UI
- **ImekaCommon**: Provides serialization utilities
- **MitkPlanarFigure**: Base classes for bounding objects

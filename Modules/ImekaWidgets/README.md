# ImekaWidgets Module

## Overview
ImekaWidgets provides **user interface components and Qt widgets** for MI-Brain. This module contains custom widgets for data visualization, ROI management, property editing, and interactive user controls.

## Primary Purpose
- Provide custom Qt widgets for MI-Brain UI
- Data node visualization and management
- ROI (Region of Interest) interactive widgets
- Property table editors
- Node selection and filtering UI components

## Core Components

### MITK-Specific Widgets

#### DataNodeGroup
**Purpose**: Manage and organize related data nodes in groups

**Key Features**:
- **Hierarchical Organization**:
  - Parent-child relationships
  - Group nodes logically
  - Maintain node collections
  - Track group membership

- **Operations**:
  - Add/remove nodes from groups
  - Query group membership
  - Iterate over group members
  - Persist group structure

- **Use Cases**:
  - Group fiber bundles by anatomical region
  - Organize ROIs by function
  - Create tract packages
  - Manage multi-subject data

**Usage**:
```cpp
// Create a group for major white matter tracts
Imeka::Widgets::DataNodeGroup majorTracts("Major Tracts");
majorTracts.AddNode(cst_left);
majorTracts.AddNode(cst_right);
majorTracts.AddNode(corpus_callosum);

// Check membership
if (majorTracts.Contains(node)) {
    std::cout << "Node is a major tract" << std::endl;
}
```

#### NodeTableViewUtils
**Purpose**: Utility functions for MITK node table visualization

**Capabilities**:
- **Table Population**:
  - Add nodes to table views
  - Configure columns
  - Set cell renderers
  - Handle selection

- **Column Configuration**:
  - Name column
  - Visibility toggle
  - Color picker
  - Opacity slider
  - Custom properties

- **Sorting and Filtering**:
  - Sort by property
  - Filter by type
  - Search by name
  - Custom predicates

- **Selection Management**:
  - Single/multi-selection
  - Selection signals
  - Programmatic selection
  - Selection persistence

**Common Functions**:
```cpp
// Populate table with fiber bundles
NodeTableViewUtils::PopulateTable(tableView, fiberNodes);

// Configure visible columns
NodeTableViewUtils::SetVisibleColumns(tableView, 
    {"Name", "Visibility", "Color", "Count"});

// Enable sorting
NodeTableViewUtils::EnableSorting(tableView, true);
```

### Qt-Specific Widgets

#### DataNodeAndPropertiesTableModel
**Purpose**: Qt model for displaying data nodes and their properties in table format

**Architecture**:
- **Model-View Pattern**: Qt's MVC architecture
  - Model: DataNodeAndPropertiesTableModel
  - View: QTableView
  - Data: MITK DataNodes with properties

**Features**:
- **Row Representation**: Each row = one data node
- **Column Representation**: Each column = one property
  
**Configurable Columns**:
  - Node name
  - Data type
  - Visibility
  - Color
  - Opacity
  - Custom properties (FA, MD, fiber count, etc.)

- **Editing Support**:
  - In-place property editing
  - Color picker integration
  - Checkbox for visibility
  - Spin box for numeric values
  - Text editing for names

- **Dynamic Updates**:
  - Automatic refresh on property changes
  - Signal emission for modifications
  - Real-time UI synchronization

**Signals**:
```cpp
Q_SIGNALS:
  void dataNodeSelected(mitk::DataNode* node);
  void propertyChanged(mitk::DataNode* node, const QString& property);
  void visibilityChanged(mitk::DataNode* node, bool visible);
```

**Usage Example**:
```cpp
// Create table model
auto model = new Imeka::Widgets::DataNodeAndPropertiesTableModel();
model->SetDataStorage(dataStorage);

// Configure displayed properties
model->SetProperties({"Name", "Visible", "Color", "Fiber Count", "Mean FA"});

// Connect to table view
QTableView* tableView = new QTableView();
tableView->setModel(model);

// Connect signals
connect(model, &DataNodeAndPropertiesTableModel::dataNodeSelected,
    [](mitk::DataNode* node) {
        std::cout << "Selected: " << node->GetName() << std::endl;
    });
```

#### RegionOfInterestWidget
**Purpose**: Interactive widget for ROI creation, editing, and management

**Key Features**:

**1. ROI Creation**:
- Sphere ROI
- Box ROI
- Ellipsoid ROI
- Cylinder ROI
- Cone ROI
- Free-form ROI

**2. ROI Manipulation**:
- Interactive placement in 3D view
- Translate (move)
- Rotate
- Scale/resize
- Deform

**3. ROI List Management**:
- Display all ROIs
- Select/deselect ROIs
- Delete ROIs
- Rename ROIs
- Change ROI properties

**4. Filtering Integration**:
- Mark as inclusion ROI
- Mark as exclusion ROI
- Apply ROI filters to fibers
- Preview filtered results

**5. Property Editing**:
- Color picker
- Opacity slider
- Size/radius adjustment
- Position coordinates
- Rotation angles

**UI Layout** (defined in RegionOfInterestWidget.ui):
- ROI type selector (dropdown)
- "Create ROI" button
- ROI list (table view)
- Property editor panel
- Filter controls
- Delete/rename buttons

**Signals**:
```cpp
Q_SIGNALS:
  void roiCreated(mitk::DataNode* roi);
  void roiSelected(mitk::DataNode* roi);
  void roiDeleted(mitk::DataNode* roi);
  void roiModified(mitk::DataNode* roi);
  void filterRequested(const std::vector<mitk::DataNode*>& rois);
```

**Usage Example**:
```cpp
// Create ROI widget
auto roiWidget = new Imeka::Widgets::RegionOfInterestWidget();
roiWidget->SetDataStorage(dataStorage);
roiWidget->SetRenderWindow(renderWindow);

// Connect to fiber filtering
connect(roiWidget, &RegionOfInterestWidget::filterRequested,
    [](const std::vector<mitk::DataNode*>& rois) {
        auto filtered = fibersManager->FilterByROIs(rois);
        DisplayFilteredFibers(filtered);
    });

// Connect to selection feedback
connect(roiWidget, &RegionOfInterestWidget::roiSelected,
    [](mitk::DataNode* roi) {
        HighlightROI(roi);
    });
```

**Workflow Example**:
1. User selects "Sphere" from dropdown
2. User clicks "Create ROI"
3. User clicks in 3D view to place sphere
4. Sphere appears at clicked location
5. ROI added to list
6. User drags sphere to desired position
7. User marks as "Inclusion ROI"
8. User clicks "Apply Filter"
9. Widget emits `filterRequested` signal
10. Application filters fibers through ROI

## Dependencies

- **MitkCore**: Core MITK functionality
- **ImekaCommon**: Common utilities
- **MitkPlanarFigure**: Planar figure support
- **ImekaGeometry**: Geometric utilities
- **ImekaBoundingObject**: ROI functionality
- **ImekaReport**: Report generation
- **Qt5|Widgets**: Qt widget framework

## Build Configuration

```cmake
MITK_CREATE_MODULE( ImekaWidgets
  PACKAGE_DEPENDS Qt5|Widgets
  DEPENDS MitkCore ImekaCommon MitkPlanarFigure 
          ImekaGeometry ImekaBoundingObject ImekaReport
  WARNINGS_NO_ERRORS
)
```

## Architecture Patterns

1. **Model-View-Controller**: Qt's MVC for table models
2. **Observer Pattern**: Signal/slot mechanism
3. **Factory Pattern**: ROI creation
4. **Decorator Pattern**: Widget enhancement
5. **Composite Pattern**: Group management

## Usage Examples

### Create Fiber Bundle Table
```cpp
// Create model
auto model = new DataNodeAndPropertiesTableModel();
model->SetDataStorage(dataStorage);

// Set properties to display
model->SetProperties({
    "Name", 
    "Visible", 
    "Color", 
    "Fiber Count", 
    "Mean FA",
    "Mean MD",
    "Length (mm)"
});

// Filter to show only fiber bundles
model->SetPredicate(mitk::NodePredicateDataType::New("FiberBundle"));

// Create view
QTableView* view = new QTableView();
view->setModel(model);
view->setSelectionMode(QAbstractItemView::SingleSelection);
view->setAlternatingRowColors(true);

// Enable editing
view->setEditTriggers(QAbstractItemView::DoubleClicked);
```

### Interactive ROI Manager
```cpp
// Create ROI widget
auto roiWidget = new RegionOfInterestWidget();
roiWidget->SetDataStorage(dataStorage);
roiWidget->SetRenderWindow(view3D);

// Enable ROI types
roiWidget->EnableROIType("Sphere", true);
roiWidget->EnableROIType("Box", true);
roiWidget->EnableROIType("Ellipsoid", true);

// Set default ROI properties
roiWidget->SetDefaultColor(Qt::red);
roiWidget->SetDefaultOpacity(0.5);
roiWidget->SetDefaultSize(5.0); // 5mm radius

// Connect to application logic
connect(roiWidget, &RegionOfInterestWidget::roiCreated,
    [](mitk::DataNode* roi) {
        qDebug() << "Created ROI:" << roi->GetName().c_str();
        LogROICreation(roi);
    });

connect(roiWidget, &RegionOfInterestWidget::filterRequested,
    [](const std::vector<mitk::DataNode*>& rois) {
        auto fibers = GetLoadedFibers();
        auto filtered = FilterFibersByROIs(fibers, rois);
        UpdateVisualization(filtered);
    });

// Add to main window
mainLayout->addWidget(roiWidget);
```

### Node Grouping for Tractography
```cpp
// Create groups for different tract categories
DataNodeGroup projection("Projection Tracts");
projection.AddNode(cst_left);
projection.AddNode(cst_right);
projection.AddNode(corticospinal);

DataNodeGroup association("Association Tracts");
association.AddNode(arcuate_left);
association.AddNode(arcuate_right);
association.AddNode(slf);

DataNodeGroup commissural("Commissural Tracts");
commissural.AddNode(corpus_callosum);
commissural.AddNode(anterior_commissure);

// Populate table with grouped view
NodeTableViewUtils::PopulateTableWithGroups(tableView, {
    &projection,
    &association,
    &commissural
});
```

### Property Table with Statistics
```cpp
// Compute statistics for fiber bundles
auto bundles = GetAllFiberBundles();
for (auto& bundle : bundles) {
    auto stats = ComputeStatistics(bundle);
    
    // Set properties for table display
    bundle->SetIntProperty("Fiber Count", stats.count);
    bundle->SetFloatProperty("Mean FA", stats.meanFA);
    bundle->SetFloatProperty("Mean MD", stats.meanMD);
    bundle->SetFloatProperty("Mean Length", stats.meanLength);
    bundle->SetFloatProperty("Volume (mm³)", stats.volume);
}

// Display in table
auto model = new DataNodeAndPropertiesTableModel();
model->SetDataStorage(dataStorage);
model->SetProperties({
    "Name",
    "Fiber Count",
    "Mean FA",
    "Mean MD",
    "Mean Length",
    "Volume (mm³)"
});

tableView->setModel(model);
```

## Key Features

- ✅ Data node table visualization
- ✅ Property editing in tables
- ✅ Interactive ROI creation
- ✅ ROI manipulation (translate, rotate, scale)
- ✅ ROI filtering integration
- ✅ Node grouping
- ✅ Qt Model-View architecture
- ✅ Signal/slot event handling
- ✅ Custom property columns
- ✅ Color picker integration
- ✅ Visibility toggles
- ✅ UI file support (.ui)
- ✅ MITK 2025 compatibility

## Module Organization

```
ImekaWidgets/
├── MITK/
│   ├── DataNodeGroup.cpp/hpp              # Node grouping
│   └── NodeTableViewUtils.cpp/hpp         # Table utilities
└── Qt/
    ├── DataNodeAndPropertiesTableModel.cpp/hpp   # Qt table model
    ├── RegionOfInterestWidget.cpp/hpp            # ROI widget
    └── RegionOfInterestWidget.ui                 # Qt Designer UI
```

## UI Integration

### Qt Designer Integration
`RegionOfInterestWidget.ui` defines the layout in Qt Designer:
- Visual layout editing
- Signal/slot connections
- Property configuration
- Localization support

### Custom Widgets in Qt Designer
ImekaWidgets components can be used as custom widgets in Qt Designer:
1. Promote QWidget to RegionOfInterestWidget
2. Set header file path
3. Use in main window layouts

## Common Use Cases

### 1. Tract Browser
Display all tractography results in a sortable, filterable table:
```cpp
auto tractBrowser = new QWidget();
auto layout = new QVBoxLayout(tractBrowser);

// Search bar
auto searchBar = new QLineEdit();
searchBar->setPlaceholderText("Search tracts...");

// Table view
auto tableView = new QTableView();
auto model = new DataNodeAndPropertiesTableModel();
model->SetProperties({"Name", "Count", "FA", "MD"});
tableView->setModel(model);

// Connect search
connect(searchBar, &QLineEdit::textChanged, [=](const QString& text) {
    model->SetFilter(text);
});

layout->addWidget(searchBar);
layout->addWidget(tableView);
```

### 2. ROI-Based Tract Segmentation
Interactive workflow for manual tract segmentation:
```cpp
auto segmentationPanel = new QWidget();
auto layout = new QVBoxLayout();

// Instructions
auto instructions = new QLabel(
    "1. Create inclusion ROIs at tract locations\n"
    "2. Create exclusion ROIs at artifact locations\n"
    "3. Click 'Extract Tract' to filter fibers"
);

// ROI widget
auto roiWidget = new RegionOfInterestWidget();

// Extract button
auto extractBtn = new QPushButton("Extract Tract");
connect(extractBtn, &QPushButton::clicked, [=]() {
    auto rois = roiWidget->GetInclusionROIs();
    auto exclusions = roiWidget->GetExclusionROIs();
    ExtractTract(rois, exclusions);
});

layout->addWidget(instructions);
layout->addWidget(roiWidget);
layout->addWidget(extractBtn);
segmentationPanel->setLayout(layout);
```

### 3. Multi-Subject Statistics Viewer
Compare metrics across multiple subjects:
```cpp
auto statsViewer = new QTableView();
auto model = new DataNodeAndPropertiesTableModel();

// Load data for multiple subjects
for (const auto& subject : subjects) {
    auto bundles = LoadSubjectBundles(subject);
    for (auto& bundle : bundles) {
        bundle->SetStringProperty("Subject", subject);
        dataStorage->Add(bundle);
    }
}

// Configure columns
model->SetProperties({
    "Subject",
    "Tract",
    "Count",
    "Mean FA",
    "Mean MD",
    "Mean Length"
});

statsViewer->setModel(model);

// Enable export
auto exportBtn = new QPushButton("Export to CSV");
connect(exportBtn, &QPushButton::clicked, [=]() {
    ExportTableToCSV(model, "statistics.csv");
});
```

### 4. Quality Control Interface
Review and mark data quality:
```cpp
auto qcInterface = new QWidget();
auto layout = new QVBoxLayout();

// Table with QC status
auto model = new DataNodeAndPropertiesTableModel();
model->SetProperties({
    "Subject",
    "Scan Date",
    "Fiber Count",
    "Mean FA",
    "QC Status",
    "Reviewer",
    "Notes"
});

auto tableView = new QTableView();
tableView->setModel(model);

// QC buttons
auto passBtn = new QPushButton("Mark as PASS");
auto failBtn = new QPushButton("Mark as FAIL");

connect(passBtn, &QPushButton::clicked, [=]() {
    auto selected = tableView->selectionModel()->currentIndex();
    auto node = model->GetNodeAtRow(selected.row());
    node->SetStringProperty("QC Status", "PASS");
    model->UpdateRow(selected.row());
});

layout->addWidget(tableView);
layout->addWidget(passBtn);
layout->addWidget(failBtn);
qcInterface->setLayout(layout);
```

## MITK 2025 Compatibility

**Qt 6 Preparation**:
- Compatible with Qt 6.5.x (via Qt5 build for now)
- Signal/slot syntax updated
- Model-view updates for Qt 6

**Rendering Integration**:
- Compatible with new MITK rendering pipeline
- Render window access updated

## What This Module Does NOT Do

- ❌ 3D rendering (handled by MITK rendering)
- ❌ Fiber filtering logic (handled by ImekaFiber)
- ❌ File I/O (handled by ImekaIO)
- ❌ Statistical computation (handled by other modules)
- ❌ Image processing (handled by MITK core)

## Related Modules

- **ImekaBoundingObject**: ROI creation backend
- **ImekaFiber**: Fiber filtering backend
- **ImekaGeometry**: Geometric utilities for ROI placement
- **ImekaReport**: Export data from tables
- **ImekaCommon**: Common utilities
- **MitkCore**: MITK data structures
- **MitkPlanarFigure**: Planar ROI support

## Signal/Slot Integration

All widgets use Qt's signal/slot mechanism for event handling:

**Common Signals**:
- `nodeSelected(mitk::DataNode*)`: Node selection changed
- `propertyChanged(mitk::DataNode*, QString)`: Property modified
- `roiCreated(mitk::DataNode*)`: New ROI created
- `roiModified(mitk::DataNode*)`: ROI geometry changed
- `filterRequested(...)`: User requested filtering

**Common Slots**:
- `updateView()`: Refresh widget display
- `setDataStorage(mitk::DataStorage*)`: Set data source
- `setSelection(mitk::DataNode*)`: Programmatic selection
- `clear()`: Clear widget contents

This enables loose coupling between UI components and application logic.

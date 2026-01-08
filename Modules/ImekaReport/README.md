# ImekaReport Module

## Overview
ImekaReport provides **reporting and data export** capabilities for MI-Brain. This module handles the generation of structured reports and export of quantitative results to CSV format for further analysis.

## Primary Purpose
- Generate structured reports from analysis results
- Export quantitative metrics to CSV files
- Document analysis workflows
- Support batch processing output
- Facilitate data analysis in external tools (Excel, R, Python)

## Core Components

### Document

#### Purpose
Create and manage structured analysis reports

**Key Features**:
- **Report Structure**:
  - Header section with metadata
  - Analysis parameters
  - Results sections
  - Statistics tables
  - Image snapshots
  - Conclusions

- **Content Management**:
  - Add text sections
  - Insert tables
  - Embed images
  - Structure hierarchically
  - Format consistently

- **Export Formats**:
  - HTML reports
  - PDF documents (if available)
  - Structured text
  - Rich-text format

**Use Cases**:
- Document tractography analysis
- Report fiber statistics
- Summarize batch processing
- Generate quality control reports
- Create analysis logs

**Example Report Sections**:
1. **Patient Information**: ID, scan date, demographics
2. **Acquisition Parameters**: Scanner, sequence, b-values
3. **Processing Steps**: Preprocessing, tractography, filtering
4. **Results**: Fiber counts, FA values, tract volumes
5. **Quality Metrics**: Signal-to-noise, artifacts, reliability
6. **Conclusions**: Clinical interpretation

### CSVWriter

#### Purpose
Export quantitative data to CSV (Comma-Separated Values) format

**Key Features**:
- **Data Types Supported**:
  - Scalar metrics (FA, MD, AD, RD)
  - Fiber bundle statistics
  - Volumetric measurements
  - Connectivity matrices
  - Time-series data
  - Multi-subject results

- **Formatting**:
  - Custom delimiters (comma, tab, semicolon)
  - Header row generation
  - Column naming
  - Precision control
  - Quote handling

- **Batch Support**:
  - Multiple subjects
  - Multiple metrics
  - Multiple ROIs
  - Multiple timepoints

**Common Export Scenarios**:
1. **Tract Statistics**:
   ```csv
   Tract,Count,Mean_FA,Mean_MD,Volume_mm3
   CST_L,12453,0.72,0.00081,15234
   CST_R,11892,0.71,0.00082,14987
   CC,23456,0.68,0.00079,28765
   ```

2. **Per-Vertex Metrics**:
   ```csv
   Vertex_ID,X,Y,Z,FA,MD,Curvature
   0,45.2,67.3,12.1,0.72,0.00081,15.3
   1,45.4,67.5,12.3,0.73,0.00080,14.8
   ```

3. **Connectivity Matrix**:
   ```csv
   Region,ROI_1,ROI_2,ROI_3,ROI_4
   ROI_1,0,1234,567,890
   ROI_2,1234,0,2345,1234
   ROI_3,567,2345,0,678
   ROI_4,890,1234,678,0
   ```

4. **Longitudinal Data**:
   ```csv
   Subject,Timepoint,Tract,FA,MD
   SUB001,Baseline,CST_L,0.72,0.00081
   SUB001,6months,CST_L,0.71,0.00082
   SUB001,12months,CST_L,0.70,0.00083
   ```

## Dependencies

- **MitkQtWidgetsExt**: Extended Qt widgets for UI integration
- No external dependencies for core CSV writing

## Build Configuration

```cmake
MITK_CREATE_MODULE( ImekaReport
  PACKAGE_DEPENDS
  DEPENDS MitkQtWidgetsExt
  WARNINGS_NO_ERRORS
)

if(BUILD_TESTING)
  add_subdirectory(Testing)
endif()
```

## Architecture Patterns

1. **Builder Pattern**: Document construction step-by-step
2. **Writer Pattern**: CSVWriter for data serialization
3. **Template Pattern**: Report templates with variable content
4. **Facade Pattern**: Simple interface to complex reporting

## Usage Examples

### Generate Tractography Report
```cpp
#include <ImekaReport/Document.hpp>

// Create report document
Imeka::Report::Document report;
report.SetTitle("Tractography Analysis Report");
report.SetDate(QDate::currentDate());

// Add patient information
report.AddSection("Patient Information");
report.AddText("Patient ID: SUB001");
report.AddText("Scan Date: 2026-01-08");

// Add processing parameters
report.AddSection("Processing Parameters");
report.AddText("Tractography Algorithm: Probabilistic");
report.AddText("Step Size: 0.5mm");
report.AddText("FA Threshold: 0.1");

// Add results table
report.AddSection("Fiber Bundle Statistics");
report.AddTable({
    {"Tract", "Count", "Mean FA", "Mean MD"},
    {"CST Left", "12453", "0.72", "0.00081"},
    {"CST Right", "11892", "0.71", "0.00082"},
    {"Corpus Callosum", "23456", "0.68", "0.00079"}
});

// Export report
report.ExportToHTML("/path/to/report.html");
```

### Export Fiber Statistics to CSV
```cpp
#include <ImekaReport/CSVWriter.hpp>

// Create CSV writer
Imeka::Report::CSVWriter writer;
writer.SetDelimiter(',');
writer.SetOutputPath("/path/to/statistics.csv");

// Add header
writer.AddHeader({"Tract", "Count", "Mean_FA", "Std_FA", "Mean_MD", "Volume"});

// Add data rows
for (auto& tract : tracts) {
    auto stats = ComputeStatistics(tract);
    writer.AddRow({
        tract->GetName(),
        std::to_string(stats.fiberCount),
        std::to_string(stats.meanFA),
        std::to_string(stats.stdFA),
        std::to_string(stats.meanMD),
        std::to_string(stats.volume)
    });
}

// Write to file
writer.Write();
```

### Export Connectivity Matrix
```cpp
// Compute connectivity between ROIs
int numROIs = 10;
std::vector<std::vector<int>> connectivity = 
    ComputeConnectivityMatrix(fibers, rois);

// Create CSV writer
Imeka::Report::CSVWriter writer;
writer.SetOutputPath("/path/to/connectivity.csv");

// Add header with ROI names
std::vector<std::string> header = {"Region"};
for (int i = 0; i < numROIs; ++i) {
    header.push_back("ROI_" + std::to_string(i));
}
writer.AddHeader(header);

// Add connectivity data
for (int i = 0; i < numROIs; ++i) {
    std::vector<std::string> row = {"ROI_" + std::to_string(i)};
    for (int j = 0; j < numROIs; ++j) {
        row.push_back(std::to_string(connectivity[i][j]));
    }
    writer.AddRow(row);
}

writer.Write();
```

### Batch Processing Report
```cpp
// Process multiple subjects
std::vector<std::string> subjects = {"SUB001", "SUB002", "SUB003"};

Imeka::Report::CSVWriter writer;
writer.SetOutputPath("/path/to/batch_results.csv");
writer.AddHeader({"Subject", "Tract", "Count", "Mean_FA", "Mean_MD"});

for (const auto& subjectID : subjects) {
    // Load subject data
    auto fibers = LoadSubjectFibers(subjectID);
    auto tracts = SegmentTracts(fibers);
    
    // Compute statistics
    for (auto& tract : tracts) {
        auto stats = ComputeStatistics(tract);
        writer.AddRow({
            subjectID,
            tract->GetName(),
            std::to_string(stats.fiberCount),
            std::to_string(stats.meanFA),
            std::to_string(stats.meanMD)
        });
    }
}

writer.Write();
std::cout << "Batch processing complete. Results saved." << std::endl;
```

### Quality Control Report
```cpp
Imeka::Report::Document qcReport;
qcReport.SetTitle("Quality Control Report");

// Add QC metrics
qcReport.AddSection("Data Quality");
qcReport.AddText($"Total Fibers: {totalCount}");
qcReport.AddText($"Valid Fibers: {validCount}");
qcReport.AddText($"Outliers Removed: {outlierCount}");
qcReport.AddText($"Quality Score: {qualityScore}%");

// Add visual QC snapshots
qcReport.AddSection("Visual Inspection");
qcReport.AddImage("/path/to/axial_view.png", "Axial View");
qcReport.AddImage("/path/to/sagittal_view.png", "Sagittal View");
qcReport.AddImage("/path/to/3d_view.png", "3D Rendering");

// Add conclusions
qcReport.AddSection("Conclusions");
if (qualityScore > 90) {
    qcReport.AddText("Data quality: EXCELLENT - proceed with analysis");
} else if (qualityScore > 70) {
    qcReport.AddText("Data quality: GOOD - minor issues detected");
} else {
    qcReport.AddText("Data quality: POOR - review acquisition parameters");
}

qcReport.ExportToHTML("/path/to/qc_report.html");
```

## Key Features

- ✅ Structured report generation
- ✅ CSV data export
- ✅ Custom delimiters
- ✅ Header management
- ✅ Batch processing support
- ✅ Multi-subject export
- ✅ Connectivity matrix export
- ✅ Longitudinal data support
- ✅ HTML report generation
- ✅ Table formatting
- ✅ Image embedding in reports
- ✅ Metadata inclusion

## Module Organization

```
ImekaReport/
├── Document.cpp/hpp      # Report generation
├── CSVWriter.cpp/hpp     # CSV export
└── Testing/              # Unit tests
```

## Testing

The module includes unit tests for:
- Document creation
- CSV writing
- Data formatting
- Delimiter handling
- Header generation

## Integration with Other Modules

### With ImekaFiber
```cpp
// Compute fiber statistics
auto fibersManager = GetFibersManager();
auto stats = fibersManager->ComputeStatistics();

// Export to CSV
CSVWriter writer;
writer.AddRow({
    "All Fibers",
    std::to_string(stats.count),
    std::to_string(stats.meanFA),
    std::to_string(stats.meanMD)
});
writer.Write();
```

### With FiberBundle
```cpp
// Get fiber bundle metrics
mitk::FiberBundle::Pointer bundle = GetBundle();
int count = bundle->GetNumFibers();
float meanLength = bundle->GetMeanFiberLength();

// Add to report
Document report;
report.AddText($"Fiber Count: {count}");
report.AddText($"Mean Length: {meanLength} mm");
```

### With ImekaWidgets
```cpp
// Generate report from UI
QPushButton* exportBtn = new QPushButton("Export Report");
connect(exportBtn, &QPushButton::clicked, [=]() {
    QString filename = QFileDialog::getSaveFileName(
        nullptr, "Save Report", "", "CSV Files (*.csv)");
    
    if (!filename.isEmpty()) {
        CSVWriter writer;
        writer.SetOutputPath(filename.toStdString());
        // ... add data
        writer.Write();
        QMessageBox::information(nullptr, "Success", 
            "Report exported successfully!");
    }
});
```

## Common Use Cases

### 1. Tractography Pipeline Output
Export results from automated tractography pipeline:
```cpp
CSVWriter writer("/results/pipeline_output.csv");
writer.AddHeader({"Subject", "Date", "Tracts_Generated", 
                  "Processing_Time", "Quality_Score"});

for (auto& result : pipelineResults) {
    writer.AddRow({
        result.subjectID,
        result.date,
        std::to_string(result.tractCount),
        std::to_string(result.processingTime),
        std::to_string(result.qualityScore)
    });
}
writer.Write();
```

### 2. Clinical Research Database Export
Format data for clinical database import:
```csv
PatientID,Visit,Region,Metric,Value,Unit
P001,Baseline,CST_L,FA,0.72,unitless
P001,Baseline,CST_L,MD,0.00081,mm2/s
P001,Baseline,CST_R,FA,0.71,unitless
P001,6mo,CST_L,FA,0.70,unitless
```

### 3. Statistical Analysis Preparation
Export data in format ready for R/Python analysis:
```cpp
// Export in long format for mixed-effects models
CSVWriter writer("/analysis/data_for_stats.csv");
writer.AddHeader({"SubjectID", "Group", "Timepoint", 
                  "Tract", "Metric", "Value"});

for (auto& measurement : allMeasurements) {
    writer.AddRow({
        measurement.subjectID,
        measurement.group,      // "Control" or "Patient"
        measurement.timepoint,  // "Baseline", "6mo", "12mo"
        measurement.tract,      // "CST_L", "CST_R", etc.
        measurement.metric,     // "FA", "MD", "AD", "RD"
        std::to_string(measurement.value)
    });
}
```

### 4. Automated QC Summary
Generate summary CSV for quality control review:
```cpp
CSVWriter qcWriter("/qc/qc_summary.csv");
qcWriter.AddHeader({"Subject", "AcqDate", "SNR", "MotionLevel", 
                    "FiberCount", "Pass"});

for (auto& scan : scans) {
    auto qc = ComputeQCMetrics(scan);
    writer.AddRow({
        scan.subjectID,
        scan.date,
        std::to_string(qc.snr),
        std::to_string(qc.motionLevel),
        std::to_string(qc.fiberCount),
        qc.pass ? "PASS" : "FAIL"
    });
}
```

## Data Export Best Practices

### 1. Use Descriptive Headers
```cpp
// Good
writer.AddHeader({"Subject_ID", "Tract_Name", "Mean_FA", "Std_FA"});

// Avoid
writer.AddHeader({"id", "name", "fa", "s"});
```

### 2. Include Units in Headers
```cpp
writer.AddHeader({
    "Length_mm",           // Length in millimeters
    "Volume_mm3",          // Volume in cubic millimeters
    "FA_unitless",         // Fractional Anisotropy (unitless)
    "MD_mm2_per_s"        // Mean Diffusivity (mm²/s)
});
```

### 3. Handle Missing Data
```cpp
std::string value = stats.hasMD ? 
    std::to_string(stats.meanMD) : "NA";
writer.AddRow({tractName, value});
```

### 4. Use Consistent Precision
```cpp
// Set precision for floating-point values
std::ostringstream oss;
oss << std::fixed << std::setprecision(4) << faValue;
writer.AddRow({tractName, oss.str()});
```

## What This Module Does NOT Do

- ❌ Statistical analysis (use R/Python/MATLAB)
- ❌ Plotting/visualization (use external tools)
- ❌ Database management (use dedicated DBMS)
- ❌ Data processing (handled by analysis modules)
- ❌ PDF generation directly (may require external library)
- ❌ Interactive reports (static documents only)

## Related Modules

- **ImekaFiber**: Provides statistics to export
- **FiberBundle**: Provides fiber metrics
- **ImekaWidgets**: Provides UI for report generation
- **MitkQtWidgetsExt**: Extended Qt widgets
- **ImekaCommon**: Common utilities

## Export Formats Summary

| Format | Use Case | Module Component |
|--------|----------|------------------|
| CSV | Quantitative data, statistical analysis | CSVWriter |
| HTML | Human-readable reports | Document |
| TXT | Plain text logs | Document |
| TSV | Tab-separated data | CSVWriter (custom delimiter) |

## Future Enhancements

Potential additions based on typical reporting needs:
- JSON export for web integration
- XML export for database import
- LaTeX report generation
- Automatic plot generation
- Interactive HTML reports
- Template-based reports

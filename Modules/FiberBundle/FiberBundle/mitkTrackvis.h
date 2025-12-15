#ifndef _TRACKVIS
#define _TRACKVIS

#include "FilteredFiberBundle.hpp"
#include <itkMetaDataDictionary.h>
#include <itkSize.h>
#include <mitkCommon.h>
#include <vtkCellArray.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyLine.h>
#include <vtkSmartPointer.h>

#include "FiberBundleExports.h"

enum ReferenceGeometryOrigin { ITSELF, ANAT, OTHERTRK, NONE };

// Structure to hold metadata of a TrackVis file
// ---------------------------------------------
struct TrackVis_header
{
  char idString[6];
  short int dim[3];
  float voxelSize[3];
  float origin[3];
  short int nbScalars;
  char scalarName[10][20];
  short int nbProperties;
  char propertyName[10][20];
  float voxToWorld[16];
  char reserved[444];
  char voxelOrder[4];
  char pad2[4];
  float imageOrientationPatient[6];
  char pad1[2];
  unsigned char invertX;
  unsigned char invertY;
  unsigned char invertZ;
  unsigned char swapXY;
  unsigned char swapYZ;
  unsigned char swapZX;
  int nbCount;
  int version;
  int hdrSize;
};

// Class to handle TrackVis files.
// -------------------------------
class FiberBundle_EXPORT TrackVisFiberReader
{
public:
  TrackVisFiberReader();
  ~TrackVisFiberReader();

  bool Create(const std::string &filename, const mitk::FilteredFiberBundle *);
  void Open(const std::string &filename);
  vtkIdType Read(mitk::FilteredFiberBundle *);
  short Append(const mitk::FilteredFiberBundle *);
  void WriteHdr();

private:
  // Loading
  mitk::Geometry3D::Pointer GetTransform(mitk::FilteredFiberBundle *);
  itk::MetaDataDictionary CreateDictionary();
  bool IsGeometryValid(mitk::BaseGeometry *);

  // Saving
  void FillHeaderWithGeometry(const mitk::FilteredFiberBundle *);
  void ExtractHeaderDataFromDictionary(const itk::MetaDataDictionary &);
  void ExtractHeaderDataFromBaseGeometry(const mitk::BaseGeometry *);
  mitk::BaseGeometry::Pointer GetTransformUsedWhenLoading(const mitk::FilteredFiberBundle *);

  // Both
  vtkSmartPointer<vtkMatrix4x4> ConvertAffine();
  vtkSmartPointer<vtkMatrix4x4> MatrixFromHeader();

  TrackVis_header m_Header;
  std::string m_Filename;
  std::fstream m_File;
  int m_ReferenceGeometryOrigin;
};

#endif

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

#include "mitkTrackvisHeader.h"
#include "FiberBundleExports.h"

enum ReferenceGeometryOrigin { ITSELF, ANAT, OTHERTRK, NONE };

// Class to handle TrackVis files.
// -------------------------------
class FIBERBUNDLE_EXPORT TrackVisFiberReader
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

#include "mitkTrackvis.h"

#include <itkMetaDataObject.h>
#include <vtkGenericCell.h>
#include <vtkPolyLine.h>

#include <mitkProgressBar.h>
#include "DataStorageUtils.hpp"
#include "FilteredFiberBundle.hpp"
#include "TrkProperties.hpp"

TrackVisFiberReader::TrackVisFiberReader()
  : m_Filename("")
  , m_ReferenceGeometryOrigin(-1)
{}

TrackVisFiberReader::~TrackVisFiberReader()
{
  if (m_File.is_open())
  {
    m_File.close();
  }
}

// Create a TrackVis file and store standard metadata. The file is ready to append fibers.
// ---------------------------------------------------------------------------------------
bool TrackVisFiberReader::Create(
  const std::string &filename,
  const mitk::FilteredFiberBundle *fib)
{
  // Some informations aren't set to default value
  FillHeaderWithGeometry(fib);
  if (m_ReferenceGeometryOrigin == NONE) { return false; }

  for (unsigned int i = 0; i < 10; ++i)
  {
    memset(m_Header.propertyName[i], '\0', 20);
    memset(m_Header.scalarName[i], '\0', 20);
  }
  memset(m_Header.reserved, '\0', 444);

  if (fib->IsInOrientationColoring())
  {
    m_Header.nbScalars = 0;
  }
  else
  {
    m_Header.nbScalars = 3;
    strcpy(m_Header.scalarName[0], "color_x");
    strcpy(m_Header.scalarName[1], "color_y");
    strcpy(m_Header.scalarName[2], "color_z");
  }

  m_Header.nbProperties = Imeka::Fiber::FillPropertiesFromPolydata(
    fib->GetFiberPolyData(), m_Header.propertyName);;

  m_Header.imageOrientationPatient[0] = 1.0;
  m_Header.imageOrientationPatient[1] = 0.0;
  m_Header.imageOrientationPatient[2] = 0.0;
  m_Header.imageOrientationPatient[3] = 0.0;
  m_Header.imageOrientationPatient[4] = 1.0;
  m_Header.imageOrientationPatient[5] = 0.0;
  m_Header.pad1[0] = '\0';
  m_Header.pad1[1] = '\0';
  m_Header.pad2[0] = 'R';
  m_Header.pad2[1] = 'A';
  m_Header.pad2[2] = 'S';
  m_Header.pad2[3] = '\0';
  m_Header.invertX = 0;
  m_Header.invertY = 0;
  m_Header.invertZ = 0;
  m_Header.swapXY = 0;
  m_Header.swapYZ = 0;
  m_Header.swapZX = 0;
  m_Header.nbCount = fib->GetNumFibers();
  m_Header.version = 2;
  m_Header.hdrSize = 1000;

  // write the header to the file
  m_File.open(filename, std::ifstream::out | std::ifstream::binary);
  if (!m_File.is_open())
  {
    printf("[ERROR] Unable to create file '%s'\n", filename.c_str());
    return false;
  }

  sprintf(m_Header.idString, "TRACK");
  m_File.write(reinterpret_cast<char *>(&m_Header), sizeof(TrackVis_header));
  this->m_Filename = filename;

  return true;
}

// Open an existing TrackVis file and read metadata information.
// The file pointer is positiond at the beginning of fibers data
// -------------------------------------------------------------
void TrackVisFiberReader::Open(const std::string &filename)
{
  m_File.open(filename, std::ifstream::in | std::ifstream::binary);
  if (!m_File.is_open())
  {
    printf("[ERROR] Unable to open file '%s'\n", filename.c_str());
    return;
  }
  this->m_Filename = filename;
  m_File.read(reinterpret_cast<char *>(&m_Header), sizeof(TrackVis_header));
}

// Append a fiber to the file
// --------------------------
short TrackVisFiberReader::Append(const mitk::FilteredFiberBundle *fiber)
{
  auto polydata = fiber->GetFiberPolyData();
  const int nbFibers = fiber->GetNumFibers();

  mitk::BaseGeometry::Pointer transform = GetTransformUsedWhenLoading(fiber);

  const bool writeColor = !fiber->IsInOrientationColoring();
  unsigned char *colors = nullptr;
  if (writeColor)
  {
    colors = static_cast<unsigned char *>(
      fiber->GetFiberColors()->GetVoidPointer(0));
  }
  const unsigned int colorCost = (writeColor) ? 2 : 1;
  const unsigned int propertiesCost = Imeka::Fiber::NbFloatsForProperties(polydata);

  // vtkPolyData::GetCell is supposed to return a pointer to a vtkPolyLine that
  // it owns but I had memory corruptions in my tests.
  auto cell = vtkSmartPointer<vtkGenericCell>::New();

  // Let's keep the same vector for all streamlines instead of allocating a
  // new vector for each streamline. This way, we might re-allocate 2 or 3
  // times max.
  std::vector<float> floatsToWrite;
  floatsToWrite.reserve(3 * colorCost * 500 + propertiesCost * nbFibers);

  for (vtkIdType idxFiber = 0; idxFiber < nbFibers; ++idxFiber)
  {
    polydata->GetCell(idxFiber, cell);
    const vtkIdType nbPoints = cell->GetNumberOfPoints();
    vtkPoints *points = cell->GetPoints();

    floatsToWrite.clear(); // Not supposed to free memory
    floatsToWrite.reserve(3 * colorCost * nbPoints + propertiesCost);

    for (unsigned int i = 0; i < nbPoints; ++i)
    {
      double *p = points->GetPoint(i);
      mitk::Point3D pt;
      pt[0] = p[0];
      pt[1] = p[1];
      pt[2] = p[2];

      if (transform)
      {
        transform->WorldToIndex(pt, pt);
      }

      floatsToWrite.push_back(pt[0]);
      floatsToWrite.push_back(pt[1]);
      floatsToWrite.push_back(pt[2]);
      if (writeColor)
      {
        floatsToWrite.push_back(static_cast<float>(*colors++));
        floatsToWrite.push_back(static_cast<float>(*colors++));
        floatsToWrite.push_back(static_cast<float>(*colors++));
        ++colors; // Skip alpha
      }
    }

    // Write the number of points and coordinates to the file
    m_File.write(reinterpret_cast<const char *>(&nbPoints), sizeof(int));

    Imeka::Fiber::AppendPropertiesToVector(polydata, idxFiber, floatsToWrite);
    const auto mem = reinterpret_cast<char *>(&(floatsToWrite.front()));
    const unsigned int nbBytes = sizeof(float) * floatsToWrite.size();
    m_File.write(mem, nbBytes);
  }

  return 0;
}

//// Read one fiber from the file
//// ----------------------------
vtkIdType TrackVisFiberReader::Read(mitk::FilteredFiberBundle *fiber)
{
  if (!DataStorageSingleton::dataStorage)
  {
    MITK_ERROR << "mitkFiberBundleXReader must know the current DataStorage";
    return -1;
  }

  // Do NOT load if there's something fishy
  if (std::string(m_Header.idString) != "TRACK"
   || m_Header.dim[0] < 0 || m_Header.dim[1] < 0 || m_Header.dim[2] < 0
   || m_Header.voxelSize[0] < 0.0 || m_Header.voxelSize[1] < 0.0 || m_Header.voxelSize[2] < 0.0
   || m_Header.nbScalars < 0 || m_Header.nbScalars > 10
   || m_Header.nbProperties < 0 || m_Header.nbProperties > 10)
  {
    MITK_ERROR << "Trk file is corrupted";
    return -2;
  }

  const auto transform = GetTransform(fiber);
  const bool validGeometry = IsGeometryValid(transform);

  const bool HasColorData = m_Header.nbScalars > 0;
  vtkSmartPointer<vtkUnsignedCharArray> colors = nullptr;
  if (HasColorData)
  {
    colors = fiber->GetNewColorArray(m_Header.nbCount * 4);
  }
  unsigned char rgba[4] = {0, 0, 0, 255};

  Imeka::Fiber::PropertiesHelperToReadTrk propertiesHelper(
    m_Header.nbProperties, m_Header.propertyName);

  const bool needProgressBar = m_Header.nbCount >= 10000;
  unsigned int streamlineIdx = 0;
  const unsigned int step = m_Header.nbCount / 100;
  if (needProgressBar)
  {
    mitk::ProgressBar::GetInstance()->Reset();
    mitk::ProgressBar::GetInstance()->AddStepsToDo(102);
  }

  int nbPoints = 0;
  const size_t nbBytesPerPoint = (3 + m_Header.nbScalars) * sizeof(float);
  auto vtkNewPoints = vtkSmartPointer<vtkPoints>::New();
  auto vtkNewCells = vtkSmartPointer<vtkCellArray>::New();
  
  auto voxToRas = MatrixFromHeader();
  auto rasToVox = vtkSmartPointer<vtkMatrix4x4>::New();
  vtkMatrix4x4::Invert(voxToRas, rasToVox);

  while (m_File.read(reinterpret_cast<char *>(&nbPoints), sizeof(int)))
  {
    if (nbPoints <= 0)
    {
      printf("[ERROR] Trying to read a fiber with %d points!\n", nbPoints);
      return -1;
    }

    ++streamlineIdx;
    if (needProgressBar && streamlineIdx % step == 0)
    {
      mitk::ProgressBar::GetInstance()->Progress();
    }

    auto container = vtkSmartPointer<vtkPolyLine>::New();

    float tmp[13]; // 3 points + 10 scalars
    for (int i = 0; i < nbPoints; i++)
    {
      if (!m_File.read(reinterpret_cast<char *>(&tmp), nbBytesPerPoint))
      {
        MITK_ERROR << "TrackVis::read: Error during read.";
        if (needProgressBar)
        {
          mitk::ProgressBar::GetInstance()->Progress(100);
        }
        return -3;
      }

      mitk::Point3D pt;
      pt[0] = tmp[0];
      pt[1] = tmp[1];
      pt[2] = tmp[2];

      // Points in TrackVis binary file are in VoxMM space (Index * Spacing) + 0.5*Spacing.
      // We convert to Index space and subtract 0.5 to align with MITK corner-based indexing.
      pt[0] = (pt[0] / m_Header.voxelSize[0]) - 0.5;
      pt[1] = (pt[1] / m_Header.voxelSize[1]) - 0.5;
      pt[2] = (pt[2] / m_Header.voxelSize[2]) - 0.5;

      // Transform from Index space to MITK LPS World Space
      if (validGeometry)
      {
        transform->IndexToWorld(pt, pt);
      }

      if (HasColorData)
      {
        rgba[0] = tmp[3];
        rgba[1] = tmp[4];
        rgba[2] = tmp[5];
        colors->InsertNextTypedTuple(rgba);
      }

      const vtkIdType id = vtkNewPoints->InsertNextPoint(pt.Begin());
      container->GetPointIds()->InsertNextId(id);
    }

    // Handle properties (if any).
    if (m_Header.nbProperties > 0)
    {
      if (!m_File.read(reinterpret_cast<char *>(&tmp), sizeof(float) * m_Header.nbProperties))
      {
        MITK_ERROR << "TrackVis::read: Error during read.";
        if (needProgressBar)
        {
          mitk::ProgressBar::GetInstance()->Progress(100);
        }
        return -4;
      }

      propertiesHelper.Save(tmp);
    }

    vtkNewCells->InsertNextCell(container);
  }

  auto fiberPolyData = vtkSmartPointer<vtkPolyData>::New();
  fiberPolyData->SetPoints(vtkNewPoints);
  fiberPolyData->SetLines(vtkNewCells);
  propertiesHelper.StoreInPolydata(fiberPolyData);

  if (needProgressBar)
  {
    mitk::ProgressBar::GetInstance()->Progress();
  }

  fiberPolyData->ComputeBounds();
  fiber->SetFiberPolyData(fiberPolyData);

  MITK_INFO << "Coordinate convention: " << m_Header.voxelOrder;

  if (HasColorData)
  {
    fiber->SetFiberColors(colors);
  }

  // 2 to avoid floating imprecision
  mitk::ProgressBar::GetInstance()->Progress(2);

  return fiberPolyData->GetNumberOfPoints();
}

mitk::Geometry3D::Pointer TrackVisFiberReader::GetTransform(mitk::FilteredFiberBundle *fiber)
{
  auto geometry = mitk::Geometry3D::New();
  fiber->SetReferenceGeometry(geometry.GetPointer());

  mitk::Point3D origin;
  mitk::Vector3D spacing;
  for (unsigned int i = 0; i < 3; ++i)
  {
    origin[i] = m_Header.origin[i];
    spacing[i] = m_Header.voxelSize[i];
    geometry->SetExtentInMM(i, m_Header.voxelSize[i] * m_Header.dim[i]);
  }
  geometry->SetOrigin(origin);
  geometry->SetSpacing(spacing);

  // To support RAS and LPS dataset, we need an affine giving an output in
  // the right frame of reference. If they aren't, a conversion to MITK
  // frame of reference is necessary
  auto voxmmToWorld = ConvertAffine();

  // The transform will bring the points as written on disk to world space
  geometry->SetIndexToWorldTransformByVtkMatrix(voxmmToWorld);

  // To save correctly the trk, we need some original information from the
  // header. We should only define the dictionary IF the affine is acceptable.
  geometry->SetMetaDataDictionary(CreateDictionary());

  return geometry;
}

void TrackVisFiberReader::WriteHdr()
{
  m_File.seekg(0);
  if (!m_File.write(reinterpret_cast<char *>(&m_Header), sizeof(TrackVis_header)))
    MITK_ERROR << "[ERROR] Problems saving the fiber!";
}

// Compute the right header to go from Voxels to WorldSpace (LPS)
// -------------------------------------------------------------
vtkSmartPointer<vtkMatrix4x4> TrackVisFiberReader::ConvertAffine()
{
  // Flip the affine to get VOX->LPSMM
  auto flipAffine = vtkSmartPointer<vtkMatrix4x4>::New();
  flipAffine->SetElement(0, 0, -1);
  flipAffine->SetElement(1, 1, -1);
  flipAffine->SetElement(2, 2, 1);

  auto voxToWorld = vtkSmartPointer<vtkMatrix4x4>::New();
  vtkMatrix4x4::Multiply4x4(flipAffine, MatrixFromHeader(), voxToWorld);

  return voxToWorld;
}

// Convert the array representing the affine to a more friendly format
vtkSmartPointer<vtkMatrix4x4> TrackVisFiberReader::MatrixFromHeader()
{
  auto matrix = vtkSmartPointer<vtkMatrix4x4>::New();
  for (unsigned int i = 0; i < 4; ++i)
  {
    for (unsigned int j = 0; j < 4; ++j)
    {
      const double value = m_Header.voxToWorld[i * 4 + j];
      matrix->SetElement(i, j, value);
    }
  }
  return matrix;
}

// Add useful information for saving process to a dictionary
itk::MetaDataDictionary TrackVisFiberReader::CreateDictionary()
{
  // Keep the original affine needed to save (only the useful one)
  itk::MetaDataDictionary dictionary;
  for (unsigned int i = 0; i < 16; ++i)
  {
    itk::EncapsulateMetaData<double>(dictionary, "a" + std::to_string(i), m_Header.voxToWorld[i]);
  }

  // Keep the field needed to save (only the useful one)
  for (unsigned int i = 0; i < 3; ++i)
  {
    itk::EncapsulateMetaData<double>(
	  dictionary, "o" + std::to_string(i), m_Header.origin[i]);
    itk::EncapsulateMetaData<double>(
	  dictionary, "vs" + std::to_string(i), m_Header.voxelSize[i]);
    itk::EncapsulateMetaData<char>(
	  dictionary, "vo" + std::to_string(i), m_Header.voxelOrder[i]);
    itk::EncapsulateMetaData<double>(
	  dictionary, "d" + std::to_string(i), m_Header.dim[i]);
  }

  return dictionary;
}

// Trk needs an appropriate header, but there are many possible situations.
void TrackVisFiberReader::FillHeaderWithGeometry(const mitk::FilteredFiberBundle *fiber)
{
  /*
  We procede in this order to create the header so it makes sense
  Case 1 :  TRK to TRK
  Case 2 :  TCK/VTK to TRK when an anatomy is loaded
  Case 3 :  TCK/VTK to TRK when another TRK is loaded
  Case 4 :  TCK/VTK to TRK when nothing else is loaded, not supported
  */

  if (auto geo = fiber->GetReferenceGeometry())
  {
    // If the reference geometry doesn't have a dictionary, it means the
    // FiberBundle does not come from a trk file.
    const auto dictionary = geo->GetMetaDataDictionary();
    if (dictionary.Find("a0") != dictionary.End())
    {
      // Case 1
      ExtractHeaderDataFromDictionary(dictionary);
      m_ReferenceGeometryOrigin = ITSELF;
      return;
    }
  }

  if (auto geo = GetTransformFromAnat())
  {
    // Case 2
    ExtractHeaderDataFromBaseGeometry(geo);
    m_ReferenceGeometryOrigin = ANAT;
    return;
  }

  if (auto geo = GetTransformFromTrk())
  {
    // Just making sure the trk's reference geometry have a valid dictionary
    const auto dictionary = geo->GetMetaDataDictionary();
    if (dictionary.Find("a0") != dictionary.End())
    {
      // Case 3
      ExtractHeaderDataFromDictionary(dictionary);
      m_ReferenceGeometryOrigin = OTHERTRK;
      return;
    }
  }

  // Case 4
  m_ReferenceGeometryOrigin = NONE;
}

// Get the appropriate geometry depending on its source
mitk::BaseGeometry::Pointer TrackVisFiberReader::GetTransformUsedWhenLoading(const mitk::FilteredFiberBundle *fiber)
{
  // The case where no geometry is available cannot reach this function

  // Depending on the header's origin (case 1-4), we need the right baseGeo
  // to go from WorldSpace->VoxMM
  if (m_ReferenceGeometryOrigin == ITSELF)
  {
    return fiber->GetReferenceGeometry();
  }

  if (m_ReferenceGeometryOrigin == ANAT)
  {
    // The image transform (in MITK's frame of reference) was used for the
    // header, but to write in VoxMM, it needs to be corrected like when
    // we load a trk, necessary for compatibility with Dipy_horizon,
    // TrackVis or FiberNavigator
    auto geo = GetTransformFromAnat();
    geo->SetIndexToWorldTransformByVtkMatrix(ConvertAffine());
    return geo;
  }

  if (m_ReferenceGeometryOrigin == OTHERTRK)
  {
    return GetTransformFromTrk();
  }

  return nullptr;
}

// Use the geometry from a trk to create the header
void TrackVisFiberReader::ExtractHeaderDataFromDictionary(const itk::MetaDataDictionary &dictionary)
{
  typedef itk::MetaDataObject<char> MetaDataCharType;
  typedef itk::MetaDataObject<double> MetaDataDoubleType;

  // Fill the header.voxToWorld to save with the original
  // file's header.voxToWorld
  for (unsigned int i = 0; i < 16; ++i)
  {
    m_Header.voxToWorld[i] =
      dynamic_cast<const MetaDataDoubleType *>(dictionary["a" + std::to_string(i)])->GetMetaDataObjectValue();
  }

  // Fill the fields to save with the original file's
  // fields (only the useful one)
  for (unsigned int i = 0; i < 3; ++i)
  {
    const auto strI = std::to_string(i);
    m_Header.origin[i] = dynamic_cast<const MetaDataDoubleType *>(
	  dictionary["o" + strI])->GetMetaDataObjectValue();
    m_Header.voxelSize[i] = dynamic_cast<const MetaDataDoubleType *>(
	  dictionary["vs" + strI])->GetMetaDataObjectValue();
    m_Header.voxelOrder[i] = dynamic_cast<const MetaDataCharType *>(
	  dictionary["vo" + strI])->GetMetaDataObjectValue();
    m_Header.dim[i] = dynamic_cast<const MetaDataDoubleType *>(
	  dictionary["d" + strI])->GetMetaDataObjectValue();
  }
  m_Header.voxelOrder[3] = '\0';
}

// Use the geometry from an Image to create the header
void TrackVisFiberReader::ExtractHeaderDataFromBaseGeometry(const mitk::BaseGeometry *geo)
{
  for (unsigned int i = 0; i < 3; i++)
  {
    m_Header.dim[i] = geo->GetExtent(i);
    m_Header.voxelSize[i] = geo->GetSpacing()[i];
    m_Header.origin[i] = 0;

    // Nifti files contain that information, but Trk files don't need it.
    // Not an issue for MI-Brain, Dipy_Horizon or TrackVis, but
    // FiberNavigator seems to care
    // m_Header.origin[i] = baseGeo->GetOrigin()[i];
  }

  // All tractograms saved from image will be put in RAS,
  // simplify loading / saving (case 2)
  sprintf(m_Header.voxelOrder, "RAS");
  m_Header.voxelOrder[3] = '\0';
  const int flip[3] = {-1, -1, 1};

  const auto i2wMatrix = geo->GetIndexToWorldTransform()->GetMatrix().GetVnlMatrix();
  const auto translation = geo->GetIndexToWorldTransform()->GetTranslation();

  for (unsigned int i = 0; i < 3; ++i)
  {
    for (unsigned int j = 0; j < 3; ++j)
    {
      const float value = i2wMatrix.get(i, j);
      m_Header.voxToWorld[i * 4 + j] = flip[i] * value;
    }
    m_Header.voxToWorld[i * 4 + 3] = flip[i] * translation[i];
  }
  m_Header.voxToWorld[12] = 0;
  m_Header.voxToWorld[13] = 0;
  m_Header.voxToWorld[14] = 0;
  m_Header.voxToWorld[15] = 1;
}

// Verify if the loaded trk file have a valid affine (heuristics only)
bool TrackVisFiberReader::IsGeometryValid(mitk::BaseGeometry *geo)
{
  const auto i2wMatrix = geo->GetIndexToWorldTransform()->GetMatrix().GetVnlMatrix();
  if (fabs(i2wMatrix.get(0, 0)) < 0.001 && fabs(i2wMatrix.get(1, 1)) < 0.001 && fabs(i2wMatrix.get(2, 2)) < 0.001)
  {
    return false;
  }

  const auto translation = geo->GetIndexToWorldTransform()->GetTranslation();
  for (unsigned int i = 0; i < 3; ++i)
  {
    if (abs(translation[i]) > 10000) { return false; }
    for (unsigned int j = 0; j < 3; ++j)
    {
      if (abs(i2wMatrix.get(i, j)) > 1000) { return false; }
    }
  }
  return true;
}

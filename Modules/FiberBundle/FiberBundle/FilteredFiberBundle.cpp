
#include "FilteredFiberBundle.hpp"

#include "DataStorageUtils.hpp"
#include <mitkImagePixelReadAccessor.h>
#include <mitkImageStatisticsHolder.h>
#include <mitkLevelWindowProperty.h>
#include <mitkLookupTable.h>
#include <mitkLookupTableProperty.h>
#include <mitkPixelTypeMultiplex.h>
#include <mitkSmartPointerProperty.h>

#include <vtkCellArray.h>
#include <vtkMath.h>
#include <vtkPolyLine.h>
#include <vtkCellArrayIterator.h>

#include <boost/progress.hpp>

#include <numeric> // Fill vector with std::itoa
#include <random>
#include <algorithm>

// used for PI
#include <itkMath.h>

static const double PI = itk::Math::pi;

namespace mitk
{

FilteredFiberBundle::FilteredFiberBundle(vtkPolyData* fiberPolyData)
  : FiberBundle(fiberPolyData)
  , m_RTT(false)
  , m_CurrentColoring(Orientation)
  , m_RealMaxFiberLength(0.0)
  {}

FilteredFiberBundle::FilteredFiberBundle(
  vtkCellArray* lines,
  vtkPoints* points,
  vtkUnsignedCharArray* colors)
  : FilteredFiberBundle([lines, points](){
    auto polydata = vtkSmartPointer<vtkPolyData>::New();
    polydata->SetPoints(points);
    polydata->SetLines(lines);
    return polydata;
  }())
{
  if (colors)
  {
    SetFiberColors(colors);
  }
}

FilteredFiberBundle::FilteredFiberBundle(const bool RTT)
  : FiberBundle(nullptr)
{
  m_RTT = RTT;
  GetFiberPolyData()->SetPoints(vtkSmartPointer<vtkPoints>::New());
  GetFiberPolyData()->SetLines(vtkSmartPointer<vtkCellArray>::New());
  m_FiberColors = GetNewColorArray(100);
}

FilteredFiberBundle::~FilteredFiberBundle()
{}

QStringList FilteredFiberBundle::Filters() const
{
  // Lets give the priority to .trk if the streamlines are colored
  const QString TRK = "Fibers TrackVis (*.trk)";
  const QString TCK = "Fibers MRtrix (*.tck)";

  QStringList filters;
  if (GetCurrentColoring() != Orientation)
  {
    filters << TRK << TCK;
  }
  else
  {
    filters << TCK << TRK;
  }
  filters << "Fibers VTK (*.fib)";

  return filters;
}

QString FilteredFiberBundle::Filter() const
{
  return Filters().join(";;");
}

void FilteredFiberBundle::ExportDataTo(
  vtkCellArray* lines,
  vtkPoints* points,
  vtkUnsignedCharArray* newColors) const
{
  if (IsRTT())
  {
    ExportDataRTT(lines, points, newColors);
  }
  else
  {
    ExportDataTo(lines, points, newColors, m_Visibility);
  }
}

void FilteredFiberBundle::ExportDataTo(
  vtkCellArray* lines,
  vtkPoints* points,
  vtkUnsignedCharArray* newColors,
  const QSet<vtkIdType>& visibility) const
{
  const auto colors = GetFiberColors().GetPointer();
  vtkPoints* thisPoints = m_FiberPolyData->GetPoints();
  float* thisPointsData = static_cast<float*>(thisPoints->GetVoidPointer(0));
  vtkCellArray* ca = m_FiberPolyData->GetLines();

  auto it = vtkSmartPointer<vtkCellArrayIterator>::Take(ca->NewIterator());
  vtkIdType currentCell = 0, colorIdx = 0;
  for (it->GoToFirstCell(); !it->IsDoneWithTraversal(); it->GoToNextCell())
  {
    vtkIdType nbPointsOnFiber;
    const vtkIdType* pts;
    it->GetCurrentCell(nbPointsOnFiber, pts);

    const bool addCell = visibility.contains(currentCell++);
    if (addCell) { lines->InsertNextCell(nbPointsOnFiber); }

    for (vtkIdType i = 0; i < nbPointsOnFiber; ++i)
    {
      float* pointData = thisPointsData + 3 * pts[i];
      if (addCell)
      {
        lines->InsertCellPoint(
          points->InsertNextPoint(pointData));

        if (newColors)
        {
          unsigned char rgba[4] = { 0, 0, 0, 0 };
          colors->GetTypedTuple(colorIdx, rgba);
          newColors->InsertNextTypedTuple(rgba);
        }
      }
      ++colorIdx;
    }
  }
}

void FilteredFiberBundle::ExportDataRTT(
  vtkCellArray *newLines,
  vtkPoints *newPoints,
  vtkUnsignedCharArray* newColors) const
{
  vtkIdType nbPoints;
  const vtkIdType *pts;
  auto polyData = GetFiberPolyData();
  auto points = static_cast<float *>(polyData->GetPoints()->GetVoidPointer(0));
  auto colors = static_cast<unsigned char *>(GetFiberColors()->GetVoidPointer(0));

  const vtkIdType nbStreamlines = polyData->GetLines()->GetNumberOfCells();
  for (vtkIdType cellID = 0; cellID < nbStreamlines; ++cellID)
  {
    polyData->GetCellPoints(cellID, nbPoints, pts);
    auto streamlinePoints = points + 3 * *pts;
    auto streamlineColors = colors + 4 * *pts;

    newLines->InsertNextCell(nbPoints);
    for (vtkIdType pointID = 0; pointID < nbPoints; ++pointID)
    {
      newLines->InsertCellPoint(newPoints->InsertNextPoint(streamlinePoints));
      streamlinePoints += 3;
      if (newColors)
      {
        newColors->InsertNextTypedTuple(streamlineColors);
        streamlineColors += 4;
      }
    }
  }
}

void FilteredFiberBundle::CalculateStatsUsingVisibility()
{
  // The first call to this method actually have the real max.
  if (m_RealMaxFiberLength == 0.0)
  {
    m_RealMaxFiberLength = m_MaxFiberLength;
  }

  m_MinFiberLength = 0.0;
  m_MaxFiberLength = 0.0;
  m_MeanFiberLength = 0.0;
  m_MedianFiberLength = 0.0;
  m_LengthStDev = 0.0;

  unsigned int nbFibers = 0;
  std::vector<float> sortedLengths;
  if (IsRTT())
  {
    double p1[3];
    double p2[3];
    m_NumFibers = nbFibers = m_FiberPolyData->GetNumberOfCells();
    if (nbFibers == 0) { return; }

    for (vtkIdType i = 0; i < nbFibers; ++i)
    {
      vtkCell* cell = m_FiberPolyData->GetCell(i);
      const vtkIdType nbPoints = cell->GetNumberOfPoints();
      vtkPoints* points = cell->GetPoints();
      float length = 0.0;
      for (vtkIdType j = 0; j < nbPoints - 1; ++j)
      {
        points->GetPoint(j, p1);
        points->GetPoint(j + 1, p2);
        length += std::sqrt((p1[0] - p2[0])*(p1[0] - p2[0]) + (p1[1] - p2[1])*(p1[1] - p2[1]) + (p1[2] - p2[2])*(p1[2] - p2[2]));
      }
      sortedLengths.push_back(length);
      m_MeanFiberLength += length;
    }
  }
  else
  {
    nbFibers = m_Visibility.size();
    if (nbFibers == 0) { return; }

    sortedLengths.reserve(nbFibers);
    for (const auto i : m_Visibility)
    {
      const auto length = m_FiberLengths[i];
      sortedLengths.push_back(length);
      m_MeanFiberLength += length;
    }
  }

  m_MeanFiberLength /= nbFibers;

  std::sort(sortedLengths.begin(), sortedLengths.end());
  m_MinFiberLength = sortedLengths[0];
  m_MaxFiberLength = sortedLengths[nbFibers - 1];

  for (const auto length : sortedLengths)
  {
    const auto diff = m_MeanFiberLength - length;
    m_LengthStDev += diff * diff;
  }

  if (m_NumFibers > 1)
  {
    m_LengthStDev /= (nbFibers - 1);
  }
  else
  {
    m_LengthStDev = 0.0;
  }
  m_LengthStDev = std::sqrt(m_LengthStDev);

  m_MedianFiberLength = sortedLengths.at(nbFibers / 2);
}

void FilteredFiberBundle::MirrorFibers(
  const BaseGeometry* anatGeo,
  const unsigned int axis)
{
  if (axis > 2) { return; }

  MITK_INFO << "Mirroring fibers";
  boost::progress_display disp(m_NumFibers);

  const float shift = 2.0 * anatGeo->GetCenter()[axis];
  vtkPoints* points = m_FiberPolyData->GetPoints();
  const vtkIdType nbPoints = m_FiberPolyData->GetNumberOfPoints();
  for (vtkIdType i = 0; i < nbPoints; ++i)
  {
    ++disp;

    mitk::Point3D p;
    points->GetPoint(i, p.Begin());
    p[axis] = -p[axis] + shift;
    points->SetPoint(i, p.Begin());
  }

  // TODO Use the 6 points of the old bb, transform them and get the new bb
  m_FiberPolyData->Modified();
  m_FiberPolyData->ComputeBounds();
  double* b = m_FiberPolyData->GetBounds();

  mitk::Geometry3D::Pointer geometry = mitk::Geometry3D::New();
  geometry->SetBounds(b);
  this->SetGeometry(geometry);
}

void FilteredFiberBundle::ApplyColorCoding(
  mitk::DataNode* node,
  const mitk::DataNode* anatNode,
  const FilteredFiberBundle::Coloring coloring)
{
  node->SetIntProperty("ColorCoding", coloring);
  switch (coloring)
  {
  case Orientation:
    ColorFibersByOrientation();
    break;
  case EndPoint:
    ColorCodingByEndPoints();
    break;
  case Curvature:
    ColorFibersByCurvature();
    break;
  case FromAnat:
    ColorFromAnat(anatNode);
    break;
  case Uniform:
  {
    float color[3];
    node->GetColor(color);
    SetFiberColors(color[0] * 255, color[1] * 255, color[2] * 255);
    break;
  }
  case FromFile:
    // Can't happen. Only the mitk::FiberBundleWriter can do this.
    break;
  }
}

void FilteredFiberBundle::ColorCodingByEndPoints()
{
  vtkPolyData* polydata = GetFiberPolyData();
  vtkPoints* points = polydata->GetPoints();
  const vtkIdType nbFibers = polydata->GetNumberOfLines();

  if (!m_FiberColors)
  {
    m_FiberColors = GetNewColorArray(polydata->GetNumberOfPoints());
  }

  vtkIdType tupleIdx = 0;
  vtkCellArray* fiberList = m_FiberPolyData->GetLines();
  fiberList->InitTraversal();
  for (vtkIdType fiberIdx = 0; fiberIdx < nbFibers; ++fiberIdx)
  {
    const vtkIdType* idList; // contains the point id's of the line
    vtkIdType nbPointsInCell; // number of points for current line
    fiberList->GetNextCell(nbPointsInCell, idList);

    mitk::Point3D start, end;
    points->GetPoint(idList[0], start.Begin());
    points->GetPoint(idList[nbPointsInCell - 1], end.Begin());

    mitk::Vector3D diff = end - start;
    diff.Normalize();

    // Add the same color for all points in this fiber
    unsigned char rgba[4] = {
      static_cast<unsigned char>(fabs(diff[0]) * 255.0),
      static_cast<unsigned char>(fabs(diff[1]) * 255.0),
      static_cast<unsigned char>(fabs(diff[2]) * 255.0),
      255 };
    for (vtkIdType pointIdx = 0; pointIdx < nbPointsInCell; ++pointIdx)
    {
      m_FiberColors->InsertTypedTuple(tupleIdx++, rgba);
    }
  }

  SetCurrentColorCoding(Coloring::EndPoint);
  m_UpdateTime2D.Modified();
  m_UpdateTime3D.Modified();
}

void FilteredFiberBundle::ColorFibersByCurvature(const bool minMaxNorm)
{
  Superclass::ColorFibersByCurvature(minMaxNorm);
  SetCurrentColorCoding(Coloring::Curvature);
}

void FilteredFiberBundle::ColorFibersByOrientation()
{
  Superclass::ColorFibersByOrientation();
  SetCurrentColorCoding(Coloring::Orientation);
}

template <typename TPixel>
void FilteredFiberBundle::ColorFibersByLUT(
  const mitk::PixelType,
  const mitk::Image* image,
  vtkLookupTable* lut,
  const mitk::LevelWindow& levelWindow)
{
  mitk::ImagePixelReadAccessor<TPixel, 3> readFAimage(image, image->GetVolumeData(0));
  vtkPolyData* polydata = GetFiberPolyData();
  vtkPoints* points = polydata->GetPoints();
  const vtkIdType nbPoints = polydata->GetNumberOfPoints();

  // Copy of vtkApplyLookupTableOnScalarsFast
  const auto realLookupTable = // 4 bytes : r, g, b, a
    reinterpret_cast<int *>(lut->GetTable()->GetPointer(0));
  const int maxIndex = lut->GetNumberOfColors() - 1;
  const auto _min = levelWindow.GetLowerWindowBound();
  const auto _max = levelWindow.GetUpperWindowBound();
  const float scale = (_max - _min > 0 ? (maxIndex + 1) / (_max - _min) : 0.0);
  const float bias = -_min * scale + 0.5f;

  for (vtkIdType i = 0; i < nbPoints; ++i)
  {
    Point3D px;
    points->GetPoint(i, px.Begin());
    if (image->GetGeometry()->IsInside(px))
    {
      itk::Index<3> itk_idx = { { 0, 0, 0 } };
      image->GetGeometry()->WorldToIndex(px, itk_idx);

      const TPixel intensity = readFAimage.GetPixelByIndex(itk_idx);

      // map to an index
      int idx = static_cast<int>(intensity * scale + bias);
      if (idx < 0)
      {
        idx = 0;
      }
      else if (idx > maxIndex)
      {
        idx = maxIndex;
      }
      m_FiberColors->InsertTypedTuple(
        i, reinterpret_cast<unsigned char*>(&realLookupTable[idx]));
    }
    else
    {
      unsigned char rgba[] = { 0, 0, 0, 255 };
      m_FiberColors->InsertTypedTuple(i, rgba);
    }
  }
}

// Copied from vtkMitkLevelWindowFilter.
template <class T>
void RGBtoHSI(T *RGB, T *HSI)
{
  T R = RGB[0], G = RGB[1], B = RGB[2], nR = (R < 0 ? 0 : (R > 255 ? 255 : R)) / 255,
    nG = (G < 0 ? 0 : (G > 255 ? 255 : G)) / 255, nB = (B < 0 ? 0 : (B > 255 ? 255 : B)) / 255,
    m = nR < nG ? (nR < nB ? nR : nB) : (nG < nB ? nG : nB),
    theta = (T)(std::acos(0.5f * ((nR - nG) + (nR - nB)) / std::sqrt(std::pow(nR - nG, 2) + (nR - nB) * (nG - nB))) *
      180 / PI),
    sum = nR + nG + nB;
  T H = 0, S = 0, I = 0;
  if (theta > 0)
    H = (nB <= nG) ? theta : 360 - theta;
  if (sum > 0)
    S = 1 - 3 / sum * m;
  I = sum / 3;
  HSI[0] = (T)H;
  HSI[1] = (T)S;
  HSI[2] = (T)I;
}

// Copied from vtkMitkLevelWindowFilter
template <class T>
void HSItoRGB(T *HSI, T *RGB)
{
  T H = (T)HSI[0], S = (T)HSI[1], I = (T)HSI[2], a = I * (1 - S), R = 0, G = 0, B = 0;
  if (H < 120)
  {
    B = a;
    R = (T)(I * (1 + S * std::cos(H * PI / 180) / std::cos((60 - H) * PI / 180)));
    G = 3 * I - (R + B);
  }
  else if (H < 240)
  {
    H -= 120;
    R = a;
    G = (T)(I * (1 + S * std::cos(H * PI / 180) / std::cos((60 - H) * PI / 180)));
    B = 3 * I - (R + G);
  }
  else
  {
    H -= 240;
    G = a;
    B = (T)(I * (1 + S * std::cos(H * PI / 180) / std::cos((60 - H) * PI / 180)));
    R = 3 * I - (G + B);
  }
  R *= 255;
  G *= 255;
  B *= 255;
  RGB[0] = (T)(R < 0 ? 0 : (R > 255 ? 255 : R));
  RGB[1] = (T)(G < 0 ? 0 : (G > 255 ? 255 : G));
  RGB[2] = (T)(B < 0 ? 0 : (B > 255 ? 255 : B));
}

void FilteredFiberBundle::ColorFibersByRGBAnat(
  const mitk::Image* image,
  const mitk::LevelWindow& levelWindow)
{
  // Copy of vtkApplyLookupTableOnRGBA
  unsigned char rgba[] = { 0, 0, 0, 0 };
  double rgb[3], hsi[3];

  typedef itk::RGBPixel<unsigned char> RGB;
  mitk::ImagePixelReadAccessor<RGB, 3> readFAimage(
    image, image->GetVolumeData(0));
  vtkPolyData* polydata = GetFiberPolyData();
  vtkPoints* points = polydata->GetPoints();
  const vtkIdType nbPoints = polydata->GetNumberOfPoints();

  // parameters for RGB level window
  const auto _min = levelWindow.GetLowerWindowBound();
  const auto _max = levelWindow.GetUpperWindowBound();
  const double scale = (_max - _min > 0 ? 255.0 / (_max - _min) : 0.0);
  const double bias = _min * scale;

  for (vtkIdType i = 0; i < nbPoints; ++i)
  {
    Point3D px;
    points->GetPoint(i, px.Begin());
    if (image->GetGeometry()->IsInside(px))
    {
      const RGB intensity = readFAimage.GetPixelByWorldCoordinates(px);
      rgb[0] = static_cast<float>(intensity[0]);
      rgb[1] = static_cast<float>(intensity[1]);
      rgb[2] = static_cast<float>(intensity[2]);

      RGBtoHSI<double>(rgb, hsi);
      hsi[2] = hsi[2] * 255.0 * scale - bias;
      hsi[2] = (hsi[2] > 255.0 ? 255 : (hsi[2] < 0.0 ? 0 : hsi[2]));
      hsi[2] /= 255.0;
      HSItoRGB<double>(hsi, rgb);

      rgba[0] = static_cast<unsigned char>(rgb[0]);
      rgba[1] = static_cast<unsigned char>(rgb[1]);
      rgba[2] = static_cast<unsigned char>(rgb[2]);
      rgba[3] = 255;
    }
    else
    {
      rgba[0] = 0; rgba[1] = 0; rgba[2] = 0; rgba[3] = 0;
    }
    m_FiberColors->InsertTypedTuple(i, rgba);
  }
}

void FilteredFiberBundle::ColorFromAnat(
  const mitk::DataNode* anatNode)
{
  if (!anatNode) { return; }

  auto lutProperty = dynamic_cast<mitk::LookupTableProperty*>(
    anatNode->GetProperty("LookupTable"));
  if (!lutProperty) { return; }

  auto levelWindowProperty = dynamic_cast<mitk::LevelWindowProperty*>(
    anatNode->GetProperty("levelwindow"));
  const auto levelWindow = levelWindowProperty->GetLevelWindow();

  const auto anat = dynamic_cast<mitk::Image*>(anatNode->GetData());
  auto rgbImage = [anat](const mitk::DataNode* anatNode) {
    auto prop = dynamic_cast<mitk::SmartPointerProperty*>(
      anatNode->GetProperty("ScilPy RGB Image"));
    if (prop)
    {
      auto scilpyRGB = dynamic_cast<mitk::Image*>(prop->GetSmartPointer().GetPointer());
      if (scilpyRGB) { return scilpyRGB; }
    }

    const auto isRGB = anat->GetDimension() == 3
      && anat->GetPixelType().GetNumberOfComponents() == 3;
    if (isRGB) { return anat; }

    return static_cast<mitk::Image*>(nullptr);
  }(anatNode);

  mitk::LookupTable::Pointer lut = lutProperty->GetLookupTable();
  if (lut->GetActiveTypeAsString() == "Grayscale" && !rgbImage)
  {
    auto newLut = mitk::LookupTable::New();
    newLut->SetType(mitk::LookupTable::JET);
    newLut->GetVtkLookupTable()->SetTableRange(
      levelWindow.GetLowerWindowBound(),
      levelWindow.GetUpperWindowBound());
    lut = newLut;
  }

  if (!m_FiberColors)
  {
    m_FiberColors = GetNewColorArray(m_FiberPolyData->GetNumberOfPoints());
  }

  if (rgbImage)
  {
    ColorFibersByRGBAnat(rgbImage, levelWindow);
  }
  else
  {
    mitkPixelTypeMultiplex3(
      ColorFibersByLUT, anat->GetPixelType(),
      anat, lut->GetVtkLookupTable(), levelWindow);
  }

  SetCurrentColorCoding(Coloring::FromAnat);
  m_UpdateTime2D.Modified();
  m_UpdateTime3D.Modified();
}

void FilteredFiberBundle::SetCurrentColorCoding(const Coloring coloring)
{
  m_CurrentColoring = coloring;
  m_IsInOrientationColoring = m_CurrentColoring == Coloring::Orientation;
}

void FilteredFiberBundle::ColorFibersByArray(vtkFloatArray* arr)
{
  float max = 0.0;
  float min = 999.0;
  const unsigned int nbFibers = GetNumFibers();
  for (unsigned int i = 0; i < nbFibers; i++)
  {
    const float v = arr->GetValue(i);
    if (v > max)
      max = v;
    else if (v < min)
      min = v;
  }
  if (fabs(max - min) < 0.00001f)
  {
    max = 1;
    min = 0;
  }

  mitk::LookupTable::Pointer mitkLookup = mitk::LookupTable::New();
  auto lookupTable = vtkSmartPointer<vtkLookupTable>::New();
  lookupTable->SetTableRange(0.0, 0.8);
  lookupTable->Build();
  mitkLookup->SetVtkLookupTable(lookupTable);
  mitkLookup->SetType(mitk::LookupTable::JET);

  const auto polydata = GetFiberPolyData();
  auto newColors = mitk::FiberBundle::GetNewColorArray(nbFibers);

  double color[3];
  unsigned char rgba[4] = { 0, 0, 0, 255 };
  for (unsigned int i = 0; i < nbFibers; i++)
  {
    const float v = (arr->GetValue(i) - min) / (max - min);
    lookupTable->GetColor(static_cast<double>(1.0 - v), color);
    rgba[0] = static_cast<unsigned char>(255.0 * color[0]);
    rgba[1] = static_cast<unsigned char>(255.0 * color[1]);
    rgba[2] = static_cast<unsigned char>(255.0 * color[2]);

    vtkCell* cell = polydata->GetCell(i);
    auto nbPoints = cell->GetNumberOfPoints();
    for (int j = 0; j < nbPoints; j++)
    {
      newColors->InsertNextTypedTuple(rgba);
    }
  }

  SetFiberColors(newColors);
}

void FilteredFiberBundle::SetFiberColors(
  float r, float g, float b, float alpha)
{
  Superclass::SetFiberColors(r, g, b, alpha);
  SetCurrentColorCoding(Coloring::Uniform);
}

void FilteredFiberBundle::SetFiberColors(
  vtkSmartPointer<vtkUnsignedCharArray> fiberColors)
{
  if (!m_FiberColors)
  {
    m_FiberColors = GetNewColorArray(fiberColors->GetActualMemorySize() / 4);
  }

  unsigned char source[4] = { 0, 0, 0, 0 };
  for (vtkIdType i = 0; i < m_FiberPolyData->GetNumberOfPoints(); ++i)
  {
    fiberColors->GetTypedTuple(i, source);
    m_FiberColors->InsertTypedTuple(i, source);
  }

  m_FiberColors->Modified();
  SetCurrentColorCoding(Coloring::FromFile);
  m_UpdateTime2D.Modified();
  m_UpdateTime3D.Modified();
}

void FilteredFiberBundle::UpdateColorCoding(
  const mitk::DataNode* anatNode)
{
  switch (GetCurrentColoring())
  {
  case Orientation:
    ColorFibersByOrientation(); break;
  case EndPoint:
    ColorCodingByEndPoints(); break;
  case Curvature:
    ColorFibersByCurvature(); break;
  case FromAnat:
    ColorFromAnat(anatNode); break;
  case Uniform:
  case FromFile:
    // Nothing to update. The color is not dependant on any positioning
    break;
  }
}

void FilteredFiberBundle::DeleteColoring() {
  m_FiberColors = nullptr;
  SetCurrentColorCoding(Coloring::Orientation);
}

void FilteredFiberBundle::Shuffle()
{
  m_FiberPolyData->BuildCells();
  const vtkIdType nbCells = m_FiberPolyData->GetNumberOfCells();
  if (nbCells == 0) { return; }

  std::vector<vtkIdType> ordering(nbCells);
  std::iota(ordering.begin(), ordering.end(), 0);
  std::random_device rd;
  std::mt19937 g(rd());
  std::shuffle(ordering.begin(), ordering.end(), g);

  auto allPointsBegin = static_cast<float*>(
    m_FiberPolyData->GetPoints()->GetVoidPointer(0));
  auto vtkNewPoints = vtkSmartPointer<vtkPoints>::New();
  auto vtkNewCells = vtkSmartPointer<vtkCellArray>::New();
  for (const auto cellID : ordering)
  {
    auto container = vtkSmartPointer<vtkPolyLine>::New();

    vtkIdType nPts;
    const vtkIdType *pts;
    m_FiberPolyData->GetCellPoints(cellID, nPts, pts);
    const float* streamlineBegin = allPointsBegin + (3 * *pts);
    for (vtkIdType j = 0; j < nPts; ++j)
    {
      container->GetPointIds()->InsertNextId(
        vtkNewPoints->InsertNextPoint(streamlineBegin + 3 * j));
    }
    vtkNewCells->InsertNextCell(container);
  }

  auto fiberColors = GetFiberColors();
  if (fiberColors)
  {
    // There's color information (loaded from a trk), we need to shuffle it too
    auto colors = static_cast<unsigned char*>(fiberColors->GetVoidPointer(0));
    auto newColors = GetNewColorArray(
      m_FiberPolyData->GetPoints()->GetNumberOfPoints());;
    for (const auto cellID : ordering)
    {
      vtkIdType nPts;
    const vtkIdType *pts;
      m_FiberPolyData->GetCellPoints(cellID, nPts, pts);
      const auto streamlineColors = colors + 4 * *pts;
      for (auto pointID = 0; pointID < nPts; ++pointID)
      {
        auto rgba = streamlineColors + 4 * pointID;
        newColors->InsertNextTypedTuple(rgba);
      }
    }
    SetFiberColors(newColors);
  }

  auto newPolyData = vtkPolyData::New();
  newPolyData->SetPoints(vtkNewPoints);
  newPolyData->SetLines(vtkNewCells);
  newPolyData->BuildCells();
  SetFiberPolyData(newPolyData);

  m_UpdateTime2D.Modified();
  m_UpdateTime3D.Modified();
}

} // namespace mitk

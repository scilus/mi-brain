// Must be the first included file or it doesn't work
#define _USE_MATH_DEFINES // for C++
#include <cmath>

#include "RTT.hpp"

#include <mitkImageAccessByItk.h>

#include <vtkCellArray.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyLine.h>

#include <itkImageRegionConstIteratorWithOnlyIndex.h>

#include "ImekaGeometry/BoundingBoxUtils.hpp"
#include "ImekaGeometry/MathUtils.hpp"
#include "ImekaGeometry/VtkImageUtils.hpp"

namespace Imeka
{

namespace Fiber
{

RTT::Parameters RTT::parameters;
TrackingMask RTT::trackingMask(parameters.minFA);

RTT::RTT()
  : m_RTTFibers(nullptr)
  , m_Points(nullptr)
  , m_Lines(nullptr)
  , m_Colors(nullptr)
  , m_TrackingMaskGeo(nullptr)
  , m_NbPointsToExceedMinLength(0)
  , m_NbPointsToExceedMaxLength(0)
  , m_LastNbFibers(0)
{}

void RTT::SetTrackingMaskGeometry(const mitk::BaseGeometry* geo)
{
  m_TrackingMaskGeo = geo;
}

void RTT::SetTensorsData(const mitk::Image*)
{

}

void RTT::SetRTTFibers(const mitk::FilteredFiberBundle* fibers)
{
  m_RTTFibers = fibers;
  m_Points = fibers->GetFiberPolyData()->GetPoints();
  m_Lines = fibers->GetFiberPolyData()->GetLines();
  m_Colors = fibers->GetFiberColors();
}

void RTT::Clear()
{
  m_Points->Reset();
  m_Lines->Reset();
  m_Colors->Reset();
}

void RTT::Init(const unsigned int nbPossibleFibers)
{
  m_NbPointsToExceedMinLength = std::max(
    3u, static_cast<unsigned int>(parameters.minLength / parameters.step) + 2);
  m_NbPointsToExceedMaxLength =
    parameters.maxLength / parameters.step + 2;
  m_LastNbFibers = nbPossibleFibers;
  Clear();
}

bool RTT::BoxIsInAnat(mitk::BaseGeometry* boxGeometry) const
{
  // Ensures that there's at least one part of the selection object
  // in the anat image
  for (unsigned int x = 0; x < 2; ++x)
  {
    for (unsigned int y = 0; y < 2; ++y)
    {
      for (unsigned int z = 0; z < 2; ++z)
      {
        const auto cornerPoint = boxGeometry->GetCornerPoint(
          static_cast<bool>(x), static_cast<bool>(y), static_cast<bool>(z));
        if (m_TrackingMaskGeo->IsInside(cornerPoint))
        {
          return true;
        }
      }
    }
  }
  return false;
}

void RTT::SeedFromSelectionObject(const SelectionObject* selectionObject)
{
  const auto boxGeometry = selectionObject->GetGeometry();
  if (!BoxIsInAnat(boxGeometry)) { return; }

  const unsigned int nbSeeds = parameters.nbSeeds;
  const unsigned int minNbSeeds = std::max(1u, nbSeeds - 1);

  Init(nbSeeds * nbSeeds * nbSeeds);

  if (nbSeeds == 1)
  {
    Seed(boxGeometry->GetCenter());
    return;
  }

  const auto bbox = Imeka::Geometry::GetBoundingBox(boxGeometry);
  const float
    stepX = (bbox[1] - bbox[0]) / minNbSeeds,
    stepY = (bbox[3] - bbox[2]) / minNbSeeds,
    stepZ = (bbox[5] - bbox[4]) / minNbSeeds;

  for (unsigned int idxX = 0; idxX < nbSeeds; ++idxX)
  {
    const float x = bbox[0] + idxX * stepX;
    for (unsigned int idxY = 0; idxY < nbSeeds; ++idxY)
    {
      const float y = bbox[2] + idxY * stepY;
      for (unsigned int idxZ = 0; idxZ < nbSeeds; ++idxZ)
      {
        const float z = bbox[4] + idxZ * stepZ;

        const mitk::Point3D seed = Point3D(x, y, z);
        if (boxGeometry->IsInside(seed))
        {
          Seed(seed);
        }
      }
    }
  }
}

void RTT::SeedFromMask(const mitk::Image* image)
{
  unsigned int nbNonNullVoxels = 0;
  AccessFixedDimensionByItk_1(image, Imeka::Geometry::GetNbNonNullVoxels, 3,
    nbNonNullVoxels);
  Init(nbNonNullVoxels);

  std::vector<mitk::Point3D> seeds;
  seeds.reserve(nbNonNullVoxels);
  AccessFixedDimensionByItk_2(image, Imeka::Geometry::GetNonNullPoints, 3,
    image->GetGeometry(), seeds);

  for (unsigned int idx = 0; idx < nbNonNullVoxels; ++idx)
  {
    Seed(seeds[idx]);
  }
}

void RTT::Seed(const mitk::Point3D& seed)
{
  const unsigned long maximaOffset = m_Maxima->GetOffset(seed);
  if (!ValidPosition(maximaOffset, trackingMask.GetOffset(seed)))
  {
    // We can't do anything at this starting point.
    return;
  }

  // Ensure that backward and forward tracking was done based on the
  // same peak.
  const auto startingDirection =
    Normalize(m_Maxima->PickDirection(maximaOffset));

  m_TmpPoints.clear();
  m_TmpColors.clear();
  ComputeHARDI(seed, Backward, -startingDirection);
  const vtkIdType nbBackwardPoints = m_TmpPoints.size();
  ComputeHARDI(seed, Forward, startingDirection);

  const unsigned int nbPoints = m_TmpPoints.size();
  if (nbPoints < m_NbPointsToExceedMinLength
   || nbPoints > m_NbPointsToExceedMaxLength)
  {
    return;
  }

  auto container = vtkSmartPointer<vtkPolyLine>::New();
  auto add_idx = [this, &container](vtkIdType i) {
    const auto& p = m_TmpPoints[i];
    container->GetPointIds()->InsertNextId(
      m_Points->InsertNextPoint(p[0], p[1], p[2]));

    const auto &c = m_TmpColors[i];
    unsigned char rgba[4] = { c[0], c[1], c[2], 255 };
    m_Colors->InsertNextTypedTuple(rgba);
  };

  // If there's a backward and forward part, then there's a duplicated point
  // and we must read from x to the left, then from x + 1 to the right. If not,
  // there's no reason to care about the ordering and we simply add them all.
  //   backward        forward
  // <-----------x----------..> With nbBackwardPoints == 5
  // 0  1  2  3  4  5  6  7 ..  Memory index
  // 4  3  2  1  0  -  5  6 ..  Actual order to add
  // We must add either index 0 and 5.
  if (nbBackwardPoints == nbPoints || nbBackwardPoints == 0)
  {
    // Add them all.
    for (vtkIdType i = 0; i < nbPoints; ++i) { add_idx(i); }
  }
  else
  {
    // Add backward, then forward. Avoid index == nbBackwardPoints.
    for (vtkIdType i = nbBackwardPoints - 1; i >= 0; --i) { add_idx(i); }
    for (vtkIdType i = nbBackwardPoints + 1; i < nbPoints; ++i) { add_idx(i); }
  }

  m_Lines->InsertNextCell(container);
}

void RTT::ComputeHARDI(
  const mitk::Point3D& seed,
  const Direction backwardForward,
  const V& startingDirection)
{
  itk::RGBPixel<unsigned char> c;
  mitk::Point3D currentPosition = seed;
  V currentDirection = startingDirection;
  while (true)
  {
    const mitk::Point3D nextPosition =
      currentPosition + (parameters.step * currentDirection);

    const unsigned long maximaOffset = m_Maxima->GetOffset(nextPosition);
    const unsigned long tmOffset = trackingMask.GetOffset(nextPosition);
    if (!ValidPosition(maximaOffset, tmOffset)) { return; }

    const auto nextDirection = GetNextDirection(
      currentDirection, maximaOffset, tmOffset, backwardForward);

    const mitk::ScalarType angle =
      180.0 * std::acos(currentDirection * nextDirection) / M_PI;
    if (angle > parameters.maxAngle) { return; }

    // We have usable data here
    m_TmpPoints.push_back(currentPosition);
    c[0] = static_cast<unsigned char>(fabs(255.0 * currentDirection[0]));
    c[1] = static_cast<unsigned char>(fabs(255.0 * currentDirection[1]));
    c[2] = static_cast<unsigned char>(fabs(255.0 * currentDirection[2]));
    m_TmpColors.push_back(c);

    if (m_TmpPoints.size() > m_NbPointsToExceedMaxLength) { return; }

    currentPosition = nextPosition;
    currentDirection = nextDirection;
  }
}

} // namespace Fiber

} // namespace Imeka

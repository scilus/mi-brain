#include "StreamlinesCutter.hpp"

#include <vtkGenericCell.h>
#include <vtkPointData.h>

#include "ImekaFiber/Filtering/common.hpp"

namespace Imeka
{

namespace Fiber
{

StreamlinesCutter::StreamlinesCutter(
  const mitk::FilteredFiberBundle* fibers,
  const SelectionObject* so,
  const bool drawOutside)
  : m_OriginalFibers(fibers)
  , m_OriginalPolyData(fibers->GetFiberPolyData())
  , m_ManageColors(fibers->IsInFromFileColoring())
  , m_OriginalColors(nullptr)
  , m_P0Color(nullptr)
  , m_P1Color(nullptr)
  , m_SO(so)
  , m_DrawOutside(drawOutside)
  , m_DrawInside(!m_DrawOutside) // Useful for readability
  , m_NewPoints(nullptr)
  , m_NewCells(nullptr)
  , m_NewColors(nullptr)
  , m_NewStreamline(nullptr)
{
  m_OBBTree = vtkSmartPointer<vtkOBBTree>::New();
  m_OBBTree->SetDataSet(Imeka::Fiber::CorrectSelectionBoxPolyData(m_SO));
  m_OBBTree->SetTolerance(0.01);
  m_OBBTree->BuildLocator();
}

mitk::FilteredFiberBundle::Pointer
StreamlinesCutter::GenerateCuttedStreamlines(
  const FiberIndexes& fiberIndexes,
  const bool cuttingTG)
{
  m_NewPoints = vtkSmartPointer<vtkPoints>::New();
  m_NewCells = vtkSmartPointer<vtkCellArray>::New();
  m_NewColors = mitk::FiberBundle::GetNewColorArray(0);

  // Each streamline is independently cutted
  for (const auto streamlineIdx : fiberIndexes)
  {
    m_OriginalColors = GetStreamlineColors(streamlineIdx);
    GenerateCuttedStreamline(streamlineIdx);
  }

  // We must not add ALL other streamlines not going through the ROI if we are
  // cutting a TG. `fiberIndexes` has already been filterd by the TG's results.
  if (!cuttingTG && m_DrawOutside)
  {
    AddUncuttedStreamlines(fiberIndexes);
  }

  auto newPolydata = vtkSmartPointer<vtkPolyData>::New();
  newPolydata->SetPoints(m_NewPoints);
  newPolydata->SetLines(m_NewCells);

  auto fibers = mitk::FilteredFiberBundle::New(newPolydata);
  if (m_ManageColors)
  {
    fibers->SetFiberColors(m_NewColors);
  }
  return fibers;
}

unsigned char* StreamlinesCutter::GetStreamlineColors(
  const vtkIdType streamlineIdx) const
{
  auto colors = static_cast<unsigned char*>(
    m_OriginalFibers->GetFiberColors()->GetVoidPointer(0));
  vtkIdType nPts;
  const vtkIdType* pts;
  m_OriginalPolyData->GetCellPoints(streamlineIdx, nPts, pts);
  return colors + 4 * *pts;
}

void StreamlinesCutter::GenerateCuttedStreamline(
  const vtkIdType streamlineIdx)
{
  mitk::Point3D p0, p1, p2;
  auto cell = vtkSmartPointer<vtkGenericCell>::New();
  m_OriginalPolyData->GetCell(streamlineIdx, cell);
  const vtkIdType nbPoints = cell->GetNumberOfPoints();

  // IsInside is reliable and should be trusted
  m_IsInside.clear();
  m_IsInside.reserve(nbPoints);
  for (vtkIdType pointPos = 0; pointPos < nbPoints; ++pointPos)
  {
    m_OriginalPolyData->GetPoint(cell->GetPointId(pointPos), p0.Begin());
    m_IsInside.push_back(m_SO->IsInside(p0));
  }

  m_NewStreamline = vtkSmartPointer<vtkPolyLine>::New();
  const vtkIdType nbPoints_m1 = nbPoints - 1;
  for (vtkIdType pointPos = 0; pointPos < nbPoints_m1; ++pointPos)
  {
    m_P0Color = m_OriginalColors + pointPos * 4;
    m_P1Color = m_OriginalColors + (pointPos + 1) * 4;
    m_OriginalPolyData->GetPoint(cell->GetPointId(pointPos), p0.Begin());
    m_OriginalPolyData->GetPoint(cell->GetPointId(pointPos + 1), p1.Begin());
    const bool p0IsInside = m_IsInside[pointPos];
    const bool p1IsInside = m_IsInside[pointPos + 1];
    bool p2IsInside = true;

    if (p0IsInside ^ m_DrawOutside)
    {
      AddPoint(p0, m_P0Color);
    }

    switch (GetLineState(p0, p1, p0IsInside, p1IsInside))
    {
    case NothingSpecial: // In -> In OR Out -> Out
      // Nothing. p0 has been added (if relevant) and p1 will be on next loop
      break;
    case Intersect: // In -> Out OR Out -> In
      if (pointPos + 2 < nbPoints)
      {
        p2IsInside = m_IsInside[pointPos + 2];
      }

      pointPos += DoIntersect(p0, p1, p0IsInside, p1IsInside, p2IsInside);
      break;
    case Traverse: // Out -> In -> Out
      pointPos += DoTraverse(p0, p1);
      break;
    }
  }

  // The last point of the streamline doesn't have a 'nextId',
  // only the points-based method is required
  if (m_NewStreamline->GetNumberOfPoints() > 0
    && m_IsInside[nbPoints_m1] ^ m_DrawOutside)
  {
    m_OriginalPolyData->GetPoint(cell->GetPointId(nbPoints_m1), p0.Begin());
    AddPoint(p0, m_OriginalColors + nbPoints_m1 * 4);

    m_NewCells->InsertNextCell(m_NewStreamline);
  }
}

vtkSmartPointer<vtkPoints> StreamlinesCutter::GetIntersectingPoints(
  const mitk::Point3D& p0, const mitk::Point3D& p1) const
{
  auto intersectingPoints = vtkSmartPointer<vtkPoints>::New();
  m_OBBTree->IntersectWithLine(
    p0.Begin(), p1.Begin(), intersectingPoints, nullptr);
  if (intersectingPoints->GetNumberOfPoints() == 2)
  {
    mitk::Point3D intersecting1, intersecting2;
    intersectingPoints->GetPoint(0, intersecting1.Begin());
    intersectingPoints->GetPoint(1, intersecting2.Begin());
    if (intersecting1.EuclideanDistanceTo(intersecting2) < 0.0001)
    {
      intersectingPoints->Reset();
      intersectingPoints->InsertNextPoint(intersecting1.Begin());
    }
  }
  return intersectingPoints;
}

StreamlinesCutter::LineState
StreamlinesCutter::GetLineState(
  const mitk::Point3D& p0, const mitk::Point3D& p1,
  const bool p0IsInside, const bool p1IsInside) const
{
  // nbIntersections is NOT reliable and should NOT be trusted too much.
  // It's either 0, 1 or 2, depending on the number of times the line crosses
  // the SO and it might not be logically consistent with p0IsInside and
  // p1IsInside. It will be wrong only when the points are near the SO.
  // When wrong, we should be using p0, p1 and maybe an intersected point.

  // There are a lof of cases but we consider only 3 families:
  // 1) In -> In OR Out -> Out
  //   a) nbInt == 0        reliable
  //   b) nbInt == [1, 2]   not reliable
  // 2) In -> Out OR Out -> In
  //   a) nbInt == [0, 2]   not reliable
  //   b) nbInt == 1        probably reliable
  // 3) Out -> In -> Out
  //   a) nbInt == [0, 1]   not reliable
  //   b) nbInt == 2        reliable
  //
  // NOTE: There's technically no way to differenciate between 1b (Out -> Out)
  // and 3a so we must check the IsInside of some middle points. It's still
  // only a heuristic though. Resulta are never gonna be perfect.

  // 1a 1b (all inside)
  if (p0IsInside && p1IsInside)
  {
    return NothingSpecial;
  }

  // 2a 2b
  if (p0IsInside != p1IsInside)
  {
    // Intersect is NEVER reliable even with 1 intesection because it could
    // be the other intersection.
    return Intersect;
  }

  //  p0 -- � -- � -- � -- p1
  mitk::Point3D quarterPoint, middlePoint, threeQuarterPoint;
  middlePoint.SetToMidPoint(p0, p1);
  quarterPoint.SetToMidPoint(p0, middlePoint);
  threeQuarterPoint.SetToMidPoint(middlePoint, p1);
  const bool probablyInside =
       m_SO->IsInside(middlePoint)
    || m_SO->IsInside(quarterPoint)
    || m_SO->IsInside(threeQuarterPoint);

  // 3a 3b
  if (!p0IsInside && probablyInside && !p1IsInside)
  {
    return Traverse;
  }

  // 1a 1b (all outside)
  return NothingSpecial;
}

vtkIdType StreamlinesCutter::DoIntersect(
  const mitk::Point3D& p0, const mitk::Point3D& p1,
  const bool p0IsInside, const bool p1IsInside, const bool p2IsInside)
{
  auto intersectingPoints = GetIntersectingPoints(p0, p1);
  const vtkIdType nbIntersections = intersectingPoints->GetNumberOfPoints();
  if (nbIntersections == 2)
  {
    if (!p0IsInside)
    {
      AddPoint(p0, intersectingPoints->GetPoint(0), p1);
    }
    if (m_DrawOutside) { NewLine(); }
    AddPoint(p0, intersectingPoints->GetPoint(1), p1);
    if (m_DrawInside) { NewLine(); }
    return 0;
  }
  if (nbIntersections == 0)
  {
    // This is not a practical state to be in. We KNOW we are passing from
    // In - Out or Out -> In but we don't have any intersection point. There
    // are 3 possibilities:
    // - p0    |p1
    // - p0|p1      (near enough to have no intersection)
    // - p0|    p1
    // Knowing that 1 point could be far from the border, we can't assume
    // anything.
    NewLine();
    return 0;
  }

  // We have only one intersection, so it should be reliable, but it might
  // not be; we need to check.
  mitk::Point3D intersectingPoint;
  intersectingPoints->GetPoint(0, intersectingPoint.Begin());
  const auto p0Distance = p0.EuclideanDistanceTo(intersectingPoint);
  const auto p1Distance = p1.EuclideanDistanceTo(intersectingPoint);
  const bool reliable =
    p1IsInside == p2IsInside || (p0Distance > 0.01 && p1Distance > 0.01);
  if (reliable)
  {
    AddPoint(p0, intersectingPoint, p1);
    if (p0IsInside ^ m_DrawOutside) { NewLine(); }
    return 0;
  }

  // Intersection point is either on p0 or p1. Check documentation block
  // in DoTraverse() for more information.
  vtkIdType advance = 0;
  if (m_DrawInside)
  {
    if (p0IsInside)
    {
      // p0 already added. It would have been better to add the
      // intersectingPoint but they are supposed to be really near
      if (p0Distance < p1Distance)
      {
        AddPoint(p1, m_P1Color);
      }
      else
      {
        AddPoint(p0, intersectingPoint, p1);
      }
      NewLine();
    }
    else
    {
      if (p0Distance < p1Distance)
      {
        AddPoint(p0, intersectingPoint, p1);
      }
      else
      {
        AddPoint(p0, m_P0Color);
      }
    }
  }
  else // Draw Outside
  {
    if (p0IsInside)
    {
      if (p0Distance < p1Distance)
      {
        AddPoint(p0, intersectingPoint, p1);
        NewLine();
      }
      else
      {
        AddPoint(p0, m_P0Color);
        NewLine();
        AddPoint(p0, intersectingPoint, p1);
      }
    }
    else
    {
      if (p0Distance < p1Distance)
      {
        AddPoint(p0, intersectingPoint, p1);
        NewLine();
        AddPoint(p1, m_P1Color);
      }
      else
      {
        NewLine();
        if (!p2IsInside)
        {
          AddPoint(p0, intersectingPoint, p1);
          // Skip next point because the intersection point is already
          // further than p0.
          ++advance;
        }
      }
    }
  }

  return advance;
}

vtkIdType StreamlinesCutter::DoTraverse(
  const mitk::Point3D& p0, const mitk::Point3D& p1)
{
  vtkIdType advance = 0;
  auto intersectingPoints = GetIntersectingPoints(p0, p1);
  const vtkIdType nbIntersections = intersectingPoints->GetNumberOfPoints();
  const bool reliable = nbIntersections == 2;
  if (reliable)
  {
    AddPoint(p0, intersectingPoints->GetPoint(0), p1);
    if (m_DrawOutside) { NewLine(); }
    AddPoint(p0, intersectingPoints->GetPoint(1), p1);
    if (m_DrawInside) { NewLine(); }
    return advance;
  }

  // Results might be unreliable but one of the intersection point is
  // right. In this case, we MUST use it because p0 or p1 might be
  // quite far from the SO's border. Both p0 and p1 are outside.
  //     -------                   -------
  //     |     |                   |     |
  // p0  i     p1        OR        p0    i  p1
  //     |     |                   |     |
  //     -------                   -------
  // p1 replaces missing      p0 replaces missing
  // intersection point.      intersection point.
  if (nbIntersections == 1)
  {
    mitk::Point3D intersectingPoint;
    intersectingPoints->GetPoint(0, intersectingPoint.Begin());
    const auto p0Distance = p0.EuclideanDistanceTo(intersectingPoint);
    const auto p1Distance = p1.EuclideanDistanceTo(intersectingPoint);
    if (m_DrawInside)
    {
      if (p0Distance < p1Distance)
      {
        AddPoint(p0, m_P0Color);
        AddPoint(p0, intersectingPoint, p1);
      }
      else
      {
        AddPoint(p0, intersectingPoint, p1);
        // p1 won't be added on next loop but we need it because it's the
        // best information we have. p1 =~= intersection point
        AddPoint(p1, m_P1Color);
        ++advance;
      }
      NewLine();
    }
    else
    {
      // p0 already added
      if (p0Distance < p1Distance)
      {
        AddPoint(p0, intersectingPoint, p1);
        NewLine();
      }
      else
      {
        NewLine();
        AddPoint(p0, intersectingPoint, p1);
      }
      // p1 will be added on next loop
    }
  }
  else // nbIntersections == 0
  {
    if (m_DrawInside)
    {
      // p1 won't be added on next loop but we need it because it's the
      // best information we have. p1 =~= intersection point
      AddPoint(p1, m_P1Color);
      ++advance;
    }
    NewLine();
  }

  return advance;
}

void StreamlinesCutter::AddUncuttedStreamlines(
  const FiberIndexes& fiberIndexes)
{
  auto cell = vtkSmartPointer<vtkGenericCell>::New();
  const auto nbStreamlines = m_OriginalPolyData->GetNumberOfCells();
  for (vtkIdType streamlineIdx = 0;
    streamlineIdx < nbStreamlines; ++streamlineIdx)
  {
    // Streamlines not selected by the SO need to be fully
    // added to the polydata
    if (!fiberIndexes.contains(streamlineIdx))
    {
      auto colors = GetStreamlineColors(streamlineIdx);

      auto streamline = vtkSmartPointer<vtkPolyLine>::New();
      m_OriginalPolyData->GetCell(streamlineIdx, cell);
      const vtkIdType nbPoints = cell->GetNumberOfPoints();
      for (vtkIdType pointIdx = 0; pointIdx < nbPoints; ++pointIdx)
      {
        const auto point =
          m_OriginalPolyData->GetPoint(cell->GetPointId(pointIdx));
        streamline->GetPointIds()->InsertNextId(
          m_NewPoints->InsertNextPoint(point));

        m_NewColors->InsertNextTypedTuple(colors);
        colors += 4;
      }
      m_NewCells->InsertNextCell(streamline);
    }
  }
}

} // namespace Fiber

} // namespace Imeka

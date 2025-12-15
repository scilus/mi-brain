
#include "common.hpp"

#include <vtkAbstractCellLocator.h>
#include <vtkIdList.h>
#include <vtkMath.h>
#include <vtkPolyLine.h>
#include <vtkTransformPolyDataFilter.h>
#include "FiberBundle/FilteredFiberBundle.hpp"

namespace Imeka
{

namespace Fiber
{

bool IntersectionExists(
  const vtkIdType currentFiberIndex,
  vtkPolyData* const fiberPolyData,
  const vtkIdType currentPointId,
  vtkAbstractCellLocator* tree,
  vtkGenericCell* cell)
{
  // Warning (Pos) is on the streamline level, (Id) is on the fiber level
  auto streamlinePoints = vtkSmartPointer<vtkIdList>::New();
  fiberPolyData->GetCellPoints(currentFiberIndex, streamlinePoints);
  const vtkIdType currentPointPosition =
    streamlinePoints->IsId(currentPointId);
  double p1[3];
  fiberPolyData->GetPoint(currentPointId, p1);

  // Make sure the previous point exist before testing segment
  bool intersectionExist = false;
  if (currentPointPosition > 0)
  {
    double p0[3];
    const vtkIdType previousPointId =
      streamlinePoints->GetId(currentPointPosition - 1);
    fiberPolyData->GetPoint(previousPointId, p0);
    intersectionExist |= IsIntersected(tree, p0, p1, cell);
  }

  // Make sure the next point exists before testing segment
  if (!intersectionExist
    && currentPointPosition + 1 < streamlinePoints->GetNumberOfIds())
  {
    double p2[3];
    const vtkIdType nextPointId =
      streamlinePoints->GetId(currentPointPosition + 1);
    fiberPolyData->GetPoint(nextPointId, p2);
    intersectionExist |= IsIntersected(tree, p1, p2, cell);
  }

  return intersectionExist;
}

// Can receive either an OBBTree (SO) or a VTKCellLocator (Surface)
bool IsIntersected(
  vtkAbstractCellLocator* tree,
  double* P0, double* P1,
  vtkGenericCell* cell)
{
  double tol = 0.0, t, x[3], pcoords[3];
  int subId; vtkIdType cellId;

  // Can't use IntersectWithLine(P0, P1, nullptr, nullptr) because
  // VTKCellLocator doesn't accept it
  return tree->IntersectWithLine(
    P0, P1, tol, t, x, pcoords, subId, cellId, cell) != 0;
}

vtkSmartPointer<vtkPolyData> CorrectSelectionBoxPolyData(
  const SelectionObject* box)
{
  auto transform = vtkSmartPointer<vtkTransform>::New();
  auto transF = vtkSmartPointer<vtkTransformPolyDataFilter>::New();

  // Use the selectionBox's linear transformation to create/initialize
  // the VTKpolydata correctly
  transform->Concatenate(box->GetGeometry()->GetVtkTransform());
  transF->SetInputData(box->GetVtkPolyData());
  transF->SetTransform(transform);
  transF->Update();

  return transF->GetOutput();
}

std::pair<double, double>
ComputeSegmentProperties(vtkPolyData* const fibers)
{
  const vtkIdType totalNbPoints = fibers->GetNumberOfPoints();
  if (!totalNbPoints)
  {
    return std::make_pair(0.0, 0.0);
  }

  auto streamline = vtkSmartPointer<vtkIdList>::New();
  double maxDistance = 0, meanDistance = 0;

  // Calculation of the mean/max distance between points of the streamlines
  const vtkIdType nbCells = fibers->GetNumberOfCells();
  for (vtkIdType i = 0; i < nbCells; i++)
  {
    fibers->GetCellPoints(i, streamline);
    const vtkIdType nbPoints = streamline->GetNumberOfIds();
    for (vtkIdType j = 0; j < nbPoints - 1; ++j)
    {
      double currentPoint[3], nextPoint[3];
      fibers->GetPoint(streamline->GetId(j), currentPoint);
      fibers->GetPoint(streamline->GetId(j + 1), nextPoint);
      const double distance = sqrt(
        vtkMath::Distance2BetweenPoints(currentPoint, nextPoint));
      maxDistance = std::max(maxDistance, distance);
      meanDistance += distance;
    }
  }

  return std::make_pair(meanDistance / totalNbPoints, maxDistance);
}

} // namespace Fiber

} // namespace Imeka

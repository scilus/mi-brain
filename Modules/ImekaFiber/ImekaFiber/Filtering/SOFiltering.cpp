
#include "SOFiltering.hpp"

#include <vtkCellLocator.h>
#include <vtkGenericCell.h>
#include <vtkIdList.h>
#include <vtkOBBTree.h>
#include <vtkPolyData.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>

#include "ImekaFiber/Filtering/common.hpp"

namespace Imeka
{

namespace Fiber
{

void SOFiltering::Update(
  const DatasetFilteringData& datasetData,
  const mitk::BaseData* ROI,
  Result& results,
  const bool precise)
{
  const auto so = dynamic_cast<const SelectionObject*>(ROI);
  if (!so) { return; }

  results = Filter(datasetData, so, precise);
}

FiberIndexes SOFiltering::FibersTouchingTheBox(
  vtkPolyData* fiberPolyData,
  vtkOctreePointLocator* octree,
  const SelectionObject* selectionObject) const
{
  auto pointsIdsInBounds = GetIdsFromBounds(octree, selectionObject, 0.0);

  // Accumulate the streamlines that are in the box
  FiberIndexes fiberIndexes;
  fiberIndexes.reserve(fiberPolyData->GetNumberOfCells());
  auto cellId = vtkSmartPointer<vtkIdList>::New();

  const vtkIdType nbPoints = pointsIdsInBounds->GetNumberOfTuples();
  for (vtkIdType i = 0; i < nbPoints; ++i)
  {
    const vtkIdType pointID = pointsIdsInBounds->GetValue(i);
    fiberPolyData->GetPointCells(pointID, cellId);
    const vtkIdType currentFiberIndex = cellId->GetId(0);

    // Easier to check if the streamlines is already selected than
    // checking if a 3D point is in the SO
    if (!fiberIndexes.contains(currentFiberIndex))
    {
      double currentPoint[3];
      fiberPolyData->GetPoint(pointID, currentPoint);
      if (selectionObject->IsInside(currentPoint))
      {
        fiberIndexes.insert(currentFiberIndex);
      }
    }
  }

  // Need to calculate again because it's a new dataset not in the DM
  const auto fibersProperties = ComputeSegmentProperties(fiberPolyData);
  double additionalBoundingMargin = fibersProperties.second / 2.0;

  EnhanceWithLineBasedResults(
    fiberPolyData, selectionObject, octree, additionalBoundingMargin,
    fiberIndexes);

  return fiberIndexes;
}

void SOFiltering::EnhanceWithLineBasedResults(
  vtkPolyData* fiberPolyData,
  const SelectionObject* selectionObject,
  vtkOctreePointLocator* octree,
  const double additionalBoundingMargin,
  FiberIndexes& fiberIndexes) const
{
  auto pointsIdsInBounds = GetIdsFromBounds(
    octree, selectionObject, additionalBoundingMargin);

  auto correctedBox = CorrectSelectionBoxPolyData(selectionObject);
  auto OBBTree = vtkSmartPointer<vtkOBBTree>::New();
  OBBTree->SetDataSet(correctedBox);
  OBBTree->BuildLocator();

  DetectIntersectionsWithBox(
    fiberIndexes, fiberPolyData, pointsIdsInBounds, OBBTree);
}

// For all the remaining points, test intersection with next points on
// streamline. Remaining points are the points inside the extended bounding
// box but outside of the selectionBox
void SOFiltering::DetectIntersectionsWithBox(
  FiberIndexes& fiberIndexes,
  vtkPolyData* const fiberPolyData,
  vtkIdTypeArray* const pointsIdsInBounds,
  vtkOBBTree* const OBBTree) const
{
  auto cellId = vtkSmartPointer<vtkIdList>::New();
  auto genericCell = vtkSmartPointer<vtkGenericCell>::New();
  const vtkIdType nbPoints = pointsIdsInBounds->GetNumberOfTuples();
  for (vtkIdType i = 0; i < nbPoints; i++)
  {
    const vtkIdType pointID = pointsIdsInBounds->GetValue(i);

    // If part of a fiber already in the index, skip the intersection test
    fiberPolyData->GetPointCells(pointID, cellId);
    const vtkIdType currentFiberIndex = cellId->GetId(0);
    if (fiberIndexes.contains(currentFiberIndex)) { continue; }

    // If one (or both) intersections tests successed,
    // add the current streamline
    const bool segmentsIntersectBox = IntersectionExists(
      currentFiberIndex, fiberPolyData, pointID, OBBTree, genericCell);
    if (segmentsIntersectBox)
    {
      fiberIndexes.insert(currentFiberIndex);
    }
  }
}

Result SOFiltering::Filter(
  const DatasetFilteringData& datasetData,
  const SelectionObject* selectionObject,
  const bool precise) const
{
  auto octree = datasetData.octreeFuture.result();
  auto pointsIds = GetIdsFromBounds(octree, selectionObject, 0.0);

  vtkPolyData* const fiberPolyData =
    dynamic_cast<vtkPolyData*>(octree->GetDataSet());
  auto presentFibers = FibersWithPointsInBox(
    fiberPolyData, selectionObject, pointsIds);

  if (precise)
  {
    EnhanceWithLineBasedResults(
      fiberPolyData, selectionObject, octree,
      datasetData.additionalBoundingMargin,
      presentFibers.anyPartThroughROI);
  }

  return Result(presentFibers);
}

SelectedByROIFiberIndexes SOFiltering::FibersWithPointsInBox(
  vtkPolyData* const fiberPolyData,
  const SelectionObject* selectionObject,
  vtkSmartPointer<vtkIdTypeArray> pointsIdsInBounds) const
{
  // Accumulate the streamlines that are in, or touch, the box
  SelectedByROIFiberIndexes fiberIndexes;
  const vtkIdType nbPoints = pointsIdsInBounds->GetNumberOfTuples();
  fiberIndexes.anyPartThroughROI.reserve(fiberPolyData->GetNumberOfCells());
  auto cellId = vtkSmartPointer<vtkIdList>::New();
  auto streamlinePoints = vtkSmartPointer<vtkIdList>::New();

  for (vtkIdType i = 0; i < nbPoints; i++)
  {
    const vtkIdType pointID = pointsIdsInBounds->GetValue(i);
    double currentPoint[3];
    fiberPolyData->GetPoint(pointID, currentPoint);

    fiberPolyData->GetPointCells(pointID, cellId);
    const vtkIdType currentFiberIndex = cellId->GetId(0);

    if (selectionObject->IsInside(currentPoint))
    {
      fiberIndexes.anyPartThroughROI.insert(currentFiberIndex);

      // Warning (Pos) is on the streamline level, (Id) is on the fiber level
      fiberPolyData->GetCellPoints(currentFiberIndex, streamlinePoints);
      const vtkIdType pointPosition = streamlinePoints->IsId(pointID);

      if (pointPosition == 0)
      {
        // First End selected
        fiberIndexes.startInROI.insert(currentFiberIndex);
      }
      if (pointPosition == streamlinePoints->GetNumberOfIds() - 1)
      {
        // Last End selected
        fiberIndexes.endInROI.insert(currentFiberIndex);
      }
    }
  }

  return fiberIndexes;
}

vtkSmartPointer<vtkIdTypeArray> SOFiltering::GetIdsFromBounds(
  vtkOctreePointLocator* octree,
  const SelectionObject* selectionObject,
  const double margin) const
{
  auto correctedBox = CorrectSelectionBoxPolyData(selectionObject);
  const auto bounds = correctedBox->GetBounds();
  double extendedBounds[] = {
    bounds[0] = bounds[0] - margin,
    bounds[1] = bounds[1] + margin,
    bounds[2] = bounds[2] - margin,
    bounds[3] = bounds[3] + margin,
    bounds[4] = bounds[4] - margin,
    bounds[5] = bounds[5] + margin,
  };

  auto pointsIdsInBounds = vtkSmartPointer<vtkIdTypeArray>::New();
  octree->FindPointsInArea(extendedBounds, pointsIdsInBounds);

  return pointsIdsInBounds;
}

} // namespace Fiber

} // namespace Imeka

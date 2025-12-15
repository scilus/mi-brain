
#ifndef IMEKA_FIBER_SOFILTERING_HPP_INCLUDED
#define IMEKA_FIBER_SOFILTERING_HPP_INCLUDED

#include "ImekaFiber/Filtering/Result.hpp"
#include "ImekaFiber/types.hpp"

#include "ImekaFiberExports.h"

class vtkOBBTree;

namespace Imeka
{

namespace Fiber
{

class ImekaFiber_EXPORT SOFiltering
{
public:
  void Update(
    const DatasetFilteringData&, const mitk::BaseData*, Result&, const bool);

  // Filtering function when cutting streamlines
  // (when the FiberBundles isn't in the dataManager)
  FiberIndexes FibersTouchingTheBox(
    vtkPolyData*, vtkOctreePointLocator*, const SelectionObject*) const;

  // Detect all fibers in contact with the selection box
  void DetectIntersectionsWithBox(
    FiberIndexes&, vtkPolyData* const,
    vtkIdTypeArray* const, vtkOBBTree* const) const;

private:
  vtkSmartPointer<vtkIdTypeArray> GetIdsFromBounds(
    vtkOctreePointLocator*, const SelectionObject*, const double) const;
  void EnhanceWithLineBasedResults(
    vtkPolyData*, const SelectionObject*, vtkOctreePointLocator*,
    const double, FiberIndexes&) const;
  Result Filter(
    const DatasetFilteringData&,
    const SelectionObject*,
    const bool = false) const;

  // Detect which fibers have points within the selection box
  SelectedByROIFiberIndexes FibersWithPointsInBox(
    vtkPolyData* const, const SelectionObject*,
    vtkSmartPointer<vtkIdTypeArray>) const;
};

} // namespace Fiber

} // namespace Imeka

#endif // IMEKA_FIBER_SOFILTERING_HPP_INCLUDED

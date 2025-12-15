
#ifndef IMEKA_FIBER_SURFACE_FILTERING_HPP_INCLUDED
#define IMEKA_FIBER_SURFACE_FILTERING_HPP_INCLUDED

#include "ImekaFiber/Filtering/Result.hpp"
#include "ImekaFiber/types.hpp"

#include "ImekaFiberExports.h"

class vtkCellLocator;
class vtkOctreePointLocator;

namespace Imeka
{

namespace Fiber
{

class ImekaFiber_EXPORT SurfaceFiltering
{
public:
  SurfaceFiltering();

  void SetAnat(const mitk::Image*);
  void Update(
    const mitk::FiberBundle*,
    const DatasetFilteringData&,
    const mitk::BaseData*,
    Result&);

private:
  bool PrepareSurface(
    const DatasetFilteringData&,
    const mitk::Image*,
    QSet<unsigned int>&,
    QSet<unsigned int>&) const;

  // Detect which fibers have points within the selection surface
  SelectedByROIFiberIndexes FibersWithPointsInSurface(
    vtkPolyData*,
    vtkIdTypeArray*,
    const QSet<unsigned int>&,
    mitk::BaseGeometry*);

  // Detect all fibers in contact with the selection surface
  void DetectIntersectionsWithSurface(
    vtkPolyData*, SelectedByROIFiberIndexes&, vtkIdTypeArray*,
    const mitk::Surface*, const QSet<unsigned int>&, mitk::BaseGeometry*);

  const mitk::Image* m_Anat;
  itk::Size<3> m_ImageSize;
};

} // namespace Fiber

} // namespace Imeka

#endif // IMEKA_FIBER_SURFACE_FILTERING_HPP_INCLUDED

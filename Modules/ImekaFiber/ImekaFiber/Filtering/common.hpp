
#ifndef IMEKA_FIBER_FILTERING_UTILS_HPP_INCLUDED
#define IMEKA_FIBER_FILTERING_UTILS_HPP_INCLUDED

#include "ImekaFiber/types.hpp"

#include <vtkIdTypeArray.h>

class vtkAbstractCellLocator;

namespace mitk { class FilteredFiberBundle; }

namespace Imeka
{

namespace Fiber
{

// There's no EXPORT because only the current module uses those functions

bool IntersectionExists(
  const vtkIdType,
  vtkPolyData* const,
  const vtkIdType,
  vtkAbstractCellLocator*,
  vtkGenericCell*);

bool IsIntersected(vtkAbstractCellLocator*, double*, double*, vtkGenericCell*);

// Apply the selectionBox's transformation to the VTK polydata, because
// the SelectionObject only has initialisation value
vtkSmartPointer<vtkPolyData> CorrectSelectionBoxPolyData(
  const SelectionObject*);

std::pair<double, double>
ComputeSegmentProperties(vtkPolyData* const fibers);

} // namespace Fiber

} // namespace Imeka

#endif // IMEKA_FIBER_FILTERING_UTILS_HPP_INCLUDED


#ifndef IMEKA_FIBER_RESULT_HPP_INCLUDED
#define IMEKA_FIBER_RESULT_HPP_INCLUDED

#include "ImekaFiber/types.hpp"

namespace Imeka
{

namespace Fiber
{

// Store the selected FiberIndexes
struct SelectedByROIFiberIndexes
{
  FiberIndexes anyPartThroughROI;
  FiberIndexes startInROI;
  FiberIndexes endInROI;
};

struct Result
{
  Result();
  explicit Result(const SelectedByROIFiberIndexes& fiberIndexes);

  bool IsUpToDate(const mitk::BaseData* ROI) const;
  FiberIndexes GetSelectedIndexes(const SelectionMode x) const;

  FiberIndexes anyPart;
  FiberIndexes eitherEnd;
  FiberIndexes bothEnds;
  FiberIndexes noEnd;

  // ROI -> Last Update
  // While an ActiveROI is disabled, the associated ROI may have been
  // modified. Since we never filter the fibers of a disabled ROI, we must
  // note the last update time.
  itk::ModifiedTimeType lastUpdate;
};

} // namespace Fiber

} // namespace Imeka

#endif // IMEKA_FIBER_RESULT_HPP_INCLUDED

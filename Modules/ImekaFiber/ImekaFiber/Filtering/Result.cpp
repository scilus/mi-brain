
#include "Result.hpp"

namespace Imeka
{

namespace Fiber
{

Result::Result()
{}

Result::Result(const SelectedByROIFiberIndexes& fiberIndexes)
{
  // Fuse the fibers indexes obtains to create the 4 different
  // segments selection options
  eitherEnd.reserve(fiberIndexes.anyPartThroughROI.size());
  bothEnds.reserve(fiberIndexes.anyPartThroughROI.size());
  anyPart.reserve(2 * fiberIndexes.anyPartThroughROI.size());
  noEnd.reserve(fiberIndexes.anyPartThroughROI.size());

  // Manually dealing with the operation on QSet is way faster than using
  // standard set operations  + / - / &
  anyPart = noEnd = fiberIndexes.anyPartThroughROI;
  eitherEnd = fiberIndexes.startInROI;
  for (const auto idx : fiberIndexes.startInROI)
  {
    anyPart.insert(idx);
    noEnd.remove(idx);
  }

  for (const auto idx : fiberIndexes.endInROI)
  {
    anyPart.insert(idx);
    eitherEnd.insert(idx);
    noEnd.remove(idx);
  }

  // Iterate over the smallest streamlines' subset
  if (fiberIndexes.endInROI.size() < fiberIndexes.startInROI.size())
  {
    for (const auto idx : fiberIndexes.endInROI)
    {
      if (fiberIndexes.startInROI.contains(idx))
      {
        bothEnds.insert(idx);
      }
    }
  }
  else
  {
    for (const auto idx : fiberIndexes.startInROI)
    {
      if (fiberIndexes.endInROI.contains(idx))
      {
        bothEnds.insert(idx);
      }
    }
  }
}

bool Result::IsUpToDate(const mitk::BaseData* ROI) const
{
  return ROI->GetMTime() <= lastUpdate;
}

FiberIndexes Result::GetSelectedIndexes(const SelectionMode x) const
{
  switch (x)
  {
  case SelectionMode::AnyPart:
    return anyPart;
  case SelectionMode::EitherEnd:
    return eitherEnd;
  case SelectionMode::BothEnds:
    return bothEnds;
  case SelectionMode::NoEnd:
    return noEnd;
  default:
    throw "Invalid SelectionMode for OutputFiberIndexes";
  }
}

} // namespace Fiber

} // namespace Imeka

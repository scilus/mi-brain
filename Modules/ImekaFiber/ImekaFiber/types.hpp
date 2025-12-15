#ifndef IMEKA_FIBER_TYPES_HPP_INCLUDED
#define IMEKA_FIBER_TYPES_HPP_INCLUDED

#include <QFuture>
#include <QSet>

#include <mitkBoundingObject.h>
#include "FiberBundle/FilteredFiberBundle.hpp"

#include <vtkOctreePointLocator.h>
#include <vtkPolyData.h>

namespace Imeka
{

namespace Fiber
{

typedef mitk::BoundingObject SelectionObject;
typedef QSet<vtkIdType> FiberIndexes;
typedef mitk::Vector3D V;

struct DatasetFilteringData {
  FiberIndexes visibleTractGroupIndexes;
  QFuture<vtkSmartPointer<vtkOctreePointLocator>> octreeFuture;
  double additionalBoundingMargin;
};

enum SelectionMode { AnyPart = 0, EitherEnd, BothEnds, NoEnd };
struct TractGroup
{
  struct Line {
    const mitk::DataNode* activeROINode;
    bool enabled;
    SelectionMode mode;
    bool isNot;
  };
  std::map<const mitk::BaseData*, Line> lines; // ROI -> Line
  bool modified;
};

inline bool operator==(const TractGroup::Line& l1, const TractGroup::Line& l2)
{
  return l1.enabled == l2.enabled
      && l1.mode == l2.mode
      && l1.isNot == l2.isNot;
}

inline bool operator!=(
  const TractGroup::Line& lhs,
  const TractGroup::Line& rhs)
{
  return !(lhs == rhs);
}

} // namespace Fiber

} // namespace Imeka

#endif // IMEKA_FIBER_TYPES_HPP_INCLUDED

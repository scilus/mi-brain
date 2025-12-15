#ifndef IMEKA_FIBER_FIBER_NODE_DATA_HPP_INCLUDED
#define IMEKA_FIBER_FIBER_NODE_DATA_HPP_INCLUDED

#include "ImekaCommon/types.hpp"
#include "ImekaFiber/Mapper/MitkFiberMapper2D.hpp"
#include "ImekaFiber/Mapper/MitkFiberMapper3D.hpp"
#include "Mapper/MapperData.hpp"

#include "ImekaFiberExports.h"

namespace Imeka
{

namespace Fiber
{

struct FiberNodeData
{
  mitk::MitkFiberMapper2D::Pointer fiberMapper2D;
  mitk::MitkFiberMapper3D::Pointer fiberMapper3D;
  FiberMapperData fiberMapperData;

  void UpdateIndices()
  {
    fiberMapper2D->UpdateIndices();
    fiberMapper3D->UpdateIndices();
  }
};

typedef QMap<const mitk::DataNode*, FiberNodeData> NodeDataMap;

void ImekaFiber_EXPORT ForEachFiberNode(
  NodeDataMap& nodesMap,
  std::function<void(mitk::DataNode*, FiberNodeData&)> f);

ConstNodes ImekaFiber_EXPORT GetFiberNodesToTransform(
  const ConstNodes& defaultNodes,
  const NodeDataMap& nodesMap);

} // namespace Fiber

} // namespace Imeka

#endif // IMEKA_FIBER_FIBER_NODE_DATA_HPP_INCLUDED

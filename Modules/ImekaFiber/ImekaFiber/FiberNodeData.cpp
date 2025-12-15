#include "FiberNodeData.hpp"

namespace Imeka
{

namespace Fiber
{

void ForEachFiberNode(
  NodeDataMap& nodesMap,
  std::function<void(mitk::DataNode*, FiberNodeData&)> f)
{
  const auto end = nodesMap.cend();
  for (auto it = nodesMap.cbegin(); it != end; ++it)
  {
    auto node = it.key();
    f(const_cast<mitk::DataNode*>(node), nodesMap[node]);
  }
}

ConstNodes GetFiberNodesToTransform(
  const ConstNodes& defaultNodes,
  const NodeDataMap& nodesMap)
{
  if (defaultNodes.size() > 0) { return defaultNodes; }

  // TODO Qt 5.6+ Use keyBegin and keyEnd
  ConstNodes nodes;
  nodes.reserve(nodesMap.size());
  for (const auto& fiberNode : nodesMap.keys())
  {
    nodes.push_back(fiberNode);
  }

  return nodes;
}

} // namespace Fiber

} // namespace Imeka

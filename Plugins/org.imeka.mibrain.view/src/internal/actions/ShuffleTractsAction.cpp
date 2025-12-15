#include "ShuffleTractsAction.hpp"

void ShuffleTractsAction::Run(const QList<mitk::DataNode::Pointer>& nodes)
{
  for (auto node : nodes)
  {
    node->SetBoolProperty("ShuffleFibers", true);
  }
}

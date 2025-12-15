
#include "DuplicateTractGroupAction.hpp"

void DuplicateTractGroupAction::Run(
  const QList<mitk::DataNode::Pointer> &selectedNodes)
{
  auto tractGroupNode = selectedNodes[0];
  tractGroupNode->SetBoolProperty("Duplicate", true);
}

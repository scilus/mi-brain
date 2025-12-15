
#include "EditActiveROIsAction.hpp"

#include "ImekaCommon/DataManager.hpp"

void EditActiveROIsAction::Run(
  const QList<mitk::DataNode::Pointer> &selectedNodes)
{
  auto tractGroupNode = selectedNodes[0];
  tractGroupNode->SetBoolProperty("EditActiveROIs", true);

  Imeka::DataManager dm(m_DS);
  if (dm.GetNodesInTree(tractGroupNode, true, false).empty())
  {
    dm.RemoveNode(tractGroupNode);
  }
}

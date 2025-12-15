
#include "CreateTractGroupAction.hpp"

void CreateTractGroupAction::Run(
  const QList<mitk::DataNode::Pointer> &selectedNodes)
{
  auto datasetNode = selectedNodes[0];
  datasetNode->SetBoolProperty("CreateNewTractGroup", true);
}

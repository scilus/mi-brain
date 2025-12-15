
#include "CreateCubeSOAction.hpp"

void CreateCubeSOAction::Run(
  const QList<mitk::DataNode::Pointer> &selectedNodes)
{
  auto datasetNode = selectedNodes[0];
  datasetNode->SetStringProperty("Create", "Cuboid");
}

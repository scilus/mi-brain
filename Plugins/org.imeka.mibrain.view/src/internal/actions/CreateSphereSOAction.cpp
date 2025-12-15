
#include "CreateSphereSOAction.hpp"

void CreateSphereSOAction::Run(
  const QList<mitk::DataNode::Pointer> &selectedNodes)
{
  auto datasetNode = selectedNodes[0];
  datasetNode->SetStringProperty("Create", "Ellipsoid");
}

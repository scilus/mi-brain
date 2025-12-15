
#include "CutFibersAbstractAction.hpp"

#include <QApplication>
#include <QMessageBox>

void CutFibersAbstractAction::Run(
  const QList<mitk::DataNode::Pointer> &selectedNodes)
{
  const auto ROINodes = selectedNodes.mid(0, selectedNodes.size() - 1);
  if (ROINodes.size() > 1)
  {
    QMessageBox::warning(QApplication::activeWindow(), "MI-Brain",
      "We haven't decided yet the proper behavior when applying many ROI "
      "to one or many datasets.\nPlease drag and drop only one ROI.");
    return;
  }
  const auto ROI = ROINodes.first();

  const auto datasetNode = selectedNodes.last();
  std::vector<mitk::DataNode::Pointer> datasetNodes{ datasetNode };

  if (datasetNode->GetName() == "Tracts")
  {
    datasetNodes =
      m_DS->GetDerivations(datasetNode)->CastToSTLConstContainer();
  }

  ROI->SetBoolProperty("CutWithMeSenpai!", true);
  for (const auto datasetNode : datasetNodes)
  {
    datasetNode->SetStringProperty("Cut", GetCutName().c_str());
  }
  ROI->GetPropertyList()->DeleteProperty("CutWithMeSenpai!");
}

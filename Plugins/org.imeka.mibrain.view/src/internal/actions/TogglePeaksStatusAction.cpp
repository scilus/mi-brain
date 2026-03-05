
#include "TogglePeaksStatusAction.hpp"

#include <mitkRenderingManager.h>

#include "ImekaCommon/DataManager.hpp"

void TogglePeaksStatusAction::Run(
  const QList<mitk::DataNode::Pointer> &selectedNodes)
{
  for (const auto& node : selectedNodes)
  {
    node->SetBoolProperty("TogglePeaks", true);
  }

  mitk::RenderingManager::GetInstance()
    ->InitializeViewsByBoundingObjects(m_DS);
}


#include "TogglePeaksStatusAction.hpp"

#include <mitkRenderingManager.h>

#include "ImekaCommon/DataManager.hpp"

void TogglePeaksStatusAction::Run(
  const QList<mitk::DataNode::Pointer> &selectedNodes)
{
  for (const auto node : selectedNodes)
  {
    node->SetBoolProperty("TogglePeaks", true);
  }

  const unsigned int nbNodes =
    Imeka::DataManager(m_DS).NumberOfVisibleObjects();
  mitk::RenderingManager::GetInstance()
    ->InitializeViewsByBoundingObjects(m_DS, nbNodes <= 1);
}

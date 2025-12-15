
#include "FlipFibersAbstractAction.hpp"

#include <mitkRenderingManager.h>

#include "ImekaGeometry/BoundingBoxUtils.hpp"
#include "utils.hpp"

void FlipFibersAbstractAction::Run(
  const QList<mitk::DataNode::Pointer> &selectedNodes)
{
  Imeka::DataManager DM(m_DS);
  const auto anatNode = GetAnatNodeWarn(DM, false);
  if (!anatNode) { return; }

  Imeka::Geometry::ReinitOnBoundsChangeScopeGuard maybeGlobalReinit(m_DS);
  for (auto node : selectedNodes)
  {
    node->SetIntProperty("MirrorFibers", GetAxis());
  }
  
  mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}
